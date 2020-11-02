//
// Created by mikhail on 11/2/20.
//

#include <TFile.h>
#include <TH1F.h>
#include <TH3F.h>
#include <string>
int main(int n_args, char** args){
  std::string file_name {"/home/mikhail/hades_particles_identification/output/mass2_vs_pt_vs_y.root"};
  auto file = TFile::Open( file_name.c_str() );
  std::string file_out_name{"out_fit.root"};
  auto file_out = TFile::Open(file_out_name.c_str(), "recreate");
  TH1F* centrality_distribtion;;
  file->GetObject("centrality_tof_rpc", centrality_distribtion);
  auto n_events = centrality_distribtion->GetEntries();
  float p=2.5f;
  TH3F* histo;
  while (p<100.0){
    std::string histo_name="mass2_vs_pT_vs_y_"+std::to_string(p);
    file->GetObject(histo_name.c_str(), histo);
    histo->Scale( 1.0 / (double) n_events );
    std::string dir_name="centrality_"+std::to_string(p);
    file_out->mkdir(dir_name.c_str());
    file_out->cd(dir_name.c_str());
    int pT_bins = 1;
    while (pT_bins < 250){
      auto pT_lo = -2.5+(pT_bins-1)*0.02;
      auto pT_hi = -2.5+(pT_bins+4)*0.02;
      std::string mass2_name{ "pT("+std::to_string(pT_lo)+","+std::to_string(pT_hi)+")" };
      auto mass2_distribution = histo->ProjectionZ(mass2_name.c_str(), 0, -1, pT_bins, pT_bins+5);
      mass2_distribution->Write();
      pT_bins+=5;
    }
    p+=5.0;
  }
  file->Close();
  file_out->Close();
  return 0;
}