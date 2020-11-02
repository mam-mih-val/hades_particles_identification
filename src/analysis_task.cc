//
// Created by mikhail on 6/16/20.
//

#include "analysis_task.h"

namespace AnalysisTree {
void AnalysisTask::Init(std::map<std::string, void *> &branch_map) {
  // linking pointers with branch fields
  event_header_ = static_cast<EventHeader *>(branch_map.at("event_header"));
  mdc_vtx_tracks_ = static_cast<Particles *>(branch_map.at("mdc_vtx_tracks"));
  meta_hits_ = static_cast<HitDetector *>(branch_map.at("meta_hits"));
  mdc_meta_matching_ = static_cast<Matching *>(branch_map.at("mdc_vtx_tracks2meta_hits"));

  // getting branch configurations, which store information about fields in branches
  auto event_header_config = config_->GetBranchConfig("event_header");
  auto mdc_vtx_tracks_config = config_->GetBranchConfig("mdc_vtx_tracks");
  auto meta_hits_config = config_->GetBranchConfig("meta_hits");

  // linking necessary for analysis fields with enumerator for fast access to them
  fields_id_.insert(std::make_pair(FIELDS::CENTRALITY, event_header_config.GetFieldId("selected_tof_rpc_hits_centrality")));
  fields_id_.insert(std::make_pair(FIELDS::PT2, event_header_config.GetFieldId("physical_trigger_2")));
  fields_id_.insert(std::make_pair(FIELDS::MASS_2, meta_hits_config.GetFieldId("mass2")));
  fields_id_.insert(std::make_pair(FIELDS::CHARGE, mdc_vtx_tracks_config.GetFieldId("charge")));

  // initializing histograms
  centrality_distribution_ = new TH1F( "centrality_tof_rpc", ";centrality (%);counts", 20, 0.0, 100 );
  float percentile{2.5f};
  while (percentile < 100.0){
    std::string name = "mass2_vs_pT_vs_y_"+std::to_string(percentile);
    mass2_vs_p_rapidity_centrality_.insert(
        std::make_pair(percentile,
                       new TH3F( name.c_str(),
                                ";y_{cm};p_{T}/z [GeV/c]; m^{2} [GeV^{2}/c^{4}]",
                                150, -0.75, 0.75,
                                250, -2.5, 2.5,
                                500, -3.0, 17.0 ) )
        );
    percentile+=5.0f;
  }
}

void AnalysisTask::Exec() {
  if( !event_header_->GetField<bool>(fields_id_.at(FIELDS::PT2)) )
    return;
  auto centrality = event_header_->GetField<float>(fields_id_.at(FIELDS::CENTRALITY));
  centrality_distribution_->Fill(centrality);
  auto n_tracks = mdc_vtx_tracks_->GetNumberOfChannels(); // number of tracks in current event
  for (size_t i = 0; i < n_tracks; ++i) { // loop over all tracks if current event
    auto track = mdc_vtx_tracks_->GetChannel(i); // getting track from track detector
    auto hit = meta_hits_->GetChannel(i); // getting matched with track hit in TOF-system
    auto mass2 = hit.GetField<float>(fields_id_.at(FIELDS::MASS_2));
    auto charge = track.GetField<int>(fields_id_.at(FIELDS::CHARGE));
    auto mom4 = track.Get4MomentumByMass( sqrtf(mass2) );
    try {
      mass2_vs_p_rapidity_centrality_.at(centrality)
          ->Fill(mom4.Rapidity()-0.74, mom4.Pt()/charge, mass2);
    } catch (std::out_of_range&) {}
  }
}

void AnalysisTask::Finish() {
  // Writing histograms to file
  centrality_distribution_->Write();
  for(auto histo : mass2_vs_p_rapidity_centrality_){
    histo.second->Write();
  }
}
} // namespace AnalysisTree