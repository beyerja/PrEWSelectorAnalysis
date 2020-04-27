// Includes from PrEW
#include "GlobalVar/Chiral.h"
#include "ToyMeas/ToyGen.h"

// Includes from PrEWUtils
#include "Setups/RKDistrSetup.h"

#include "spdlog/spdlog.h"

#include <fstream>
#include <iostream>

int main (int /*argc*/, char **/*argv*/) {
  spdlog::set_level(spdlog::level::info);
  
  int energy = 250;
  std::string output_path = "../output/selection_result.out";
  
  spdlog::info("Start test.");
  
  spdlog::info("Create setup.");
  PrEWUtils::Setups::RKDistrSetup setup {};
  
  spdlog::info("Add files and energies.");
  setup.add_input_file("/home/jakob/Documents/DESY/programming/TGC/PrEW/testdata/RK_examplefile_500_250_2018_04_03.root");
  setup.add_input_file("/home/jakob/Documents/DESY/MountPoints/DUSTMount/TGCAnalysis/SampleProduction/WW_charge_separated/distributions/combined/Distribution_250GeV_WW_semilep_MuAntiNu.root");
  setup.add_input_file("/home/jakob/Documents/DESY/MountPoints/DUSTMount/TGCAnalysis/SampleProduction/WW_charge_separated/distributions/combined/Distribution_250GeV_WW_semilep_AntiMuNu.root");
  setup.add_input_file("/home/jakob/Documents/DESY/MountPoints/DUSTMount/TGCAnalysis/SampleProduction/Z_2f_separated/PrEW_inputs/Z2f_separated_cosTheta_distributions.root");
  setup.add_energy( energy );

  spdlog::info("Selecting distributions.");
  setup.use_distr("singleWplussemileptonic");
  setup.use_distr("singleWminussemileptonic");
  setup.use_distr("WWsemileptonic");
  setup.use_distr("WW_semilep_MuAntiNu");
  setup.use_distr("WW_semilep_AntiMuNu");
  setup.use_distr("ZZsemileptonic");
  setup.use_distr("Zhadronic");
  setup.use_distr("Zleptonic");
  setup.use_distr("Zuds_81to101");
  setup.use_distr("Zuds_180to275");
  setup.use_distr("Zcc_81to101");
  setup.use_distr("Zcc_180to275");
  setup.use_distr("Zbb_81to101");
  setup.use_distr("Zbb_180to275");
  setup.use_distr("Zmumu_81to101");
  setup.use_distr("Zmumu_180to275");
  
  spdlog::info("Setting up linking info.");
  
  // Set up the systematics
  setup.set_lumi(energy, 2000, 1);
  setup.add_pol("ePol-", energy, 0.8, 0.0001);
  setup.add_pol("ePol+", energy, 0.8, 0.0001);
  setup.add_pol("pPol-", energy, 0.3, 0.0001);
  setup.add_pol("pPol+", energy, 0.3, 0.0001);
  
  // Set up the desired polarisation sharings
  setup.add_pol_config("e-p+", energy, "ePol-", "pPol+", "-", "+", 0.45);
  setup.add_pol_config("e+p-", energy, "ePol+", "pPol-", "+", "-", 0.45);
  setup.add_pol_config("e-p-", energy, "ePol-", "pPol-", "-", "-", 0.05);
  setup.add_pol_config("e+p+", energy, "ePol+", "pPol+", "+", "+", 0.05);
  
  // Make sure WW & ZZ have correct normalisations
  setup.set_WW_mu_only();
  setup.set_ZZ_mu_only();
  
  spdlog::info("Finalizing linking info.");
  setup.complete_setup(); // This must come last in linking setup

  spdlog::info("Get expected (not fluctuated) distributions using toy generator.");
  PREW::ToyMeas::ToyGen toy_gen (setup.get_data_connector(),setup.get_pars());
  auto distrs = toy_gen.get_expected_distrs(energy);

  spdlog::info("Open output file.");
  std::ofstream outfile;
  outfile.open( output_path.c_str(), std::ios::trunc ); // Open file
  if (! outfile.is_open()) {
    spdlog::error("Couldn't open output file {}", output_path);
    return 1;
  }
  
  spdlog::info("Write to file.");
  outfile << "Energy = " + std::to_string(energy) + "\n\n";
  
  for (const auto & distr : distrs) {
    outfile << "<=====DISTR-BEGIN=====>\n";
    outfile << "Name: " << distr.m_info.m_distr_name << "\n";
    outfile << "PolConfig: " << distr.m_info.m_pol_config << "\n";
    
    int n_bins = distr.m_bin_centers.size();
    int n_dims = distr.m_bin_centers[0].size();
    outfile << "NBins: " << std::to_string(n_bins) << "\n";
    outfile << "Dim: " << std::to_string(n_dims) << "\n";
    
    outfile << "Bin-ID ";
    for (int d=0; d<n_dims; d++) { outfile << "d" << std::to_string(d) << " ";}
    outfile << " val\n";
    for (int b=0; b<n_bins; b++) {
      outfile << "B" << std::to_string(b) << " ";
      for (double x: distr.m_bin_centers[b]) { 
        outfile << std::to_string(x) << " ";
      }
      outfile << std::to_string(distr.m_distribution[b].get_val_mst()) << "\n";
    }
    
    outfile << "<=====DISTR-END=======>\n\n";
  }
  
  spdlog::info("Close output file.");
  outfile.close();
  
  spdlog::info("Done!");
}