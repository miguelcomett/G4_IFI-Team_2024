// ROOT macro file for plotting example B4 histograms
//
// Can be run from ROOT session:
// root[0] .x plotHisto.C

{
  gROOT->Reset();
  gROOT->SetStyle("Plain"); /*Plain */

  // Draw histos filled by Geant4 simulation
  //

  // Open file filled by Geant4 simulation
  TFile f("spectra.root");
  ofstream fout("geant4.txt");
  // Create a canvas and divide it into 2x2 pads
  TCanvas *c1 = new TCanvas("c1", "", 20, 20, 1000, 1000);
  c1->Divide(1, 1);
  c1->SetGrid();

  // Draw Eabs histogram in the pad 1
  c1->cd(1);
  TH1D *hist1 = (TH1D *)f.Get("h0");

  for (int i = 0; i <= hist1->GetNbinsX(); i++)
  {

    fout << hist1->GetXaxis()->GetBinCenter(i) + 1 << " " << hist1->GetBinContent(i) << endl;
  }

  fout.close();

  hist1->SetLineColor(4);
  hist1->SetLineWidth(2);
  // hist1->SetMarkerColor(4);
  // hist1->SetMarkerSize(1.5);
  // hist1->SetMarkerStyle(21);
  hist1->SetTitle("");
  hist1->GetXaxis()->SetTitle("Energy [keV]");
  hist1->GetYaxis()->SetTitle("Counts [a.u]");
  hist1->Draw("h0");
  hist1->Print("h0");
  c1->Print("name.png");

  // Draw Labs histogram in the pad 2
  // c1->cd(2);
  // TH1D* hist2 = (TH1D*)f.Get("Labs");
  // hist2->Draw("HIST");

  // Draw Egap histogram in the pad 3
  // with logaritmic scale for y
  // TH1D* hist3 = (TH1D*)f.Get("Egap");
  // c1->cd(3);
  // gPad->SetLogy(1);
  // hist3->Draw("HIST");

  // Draw Lgap histogram in the pad 4
  // with logaritmic scale for y
  // c1->cd(4);
  // gPad->SetLogy(1);
  // TH1D* hist4 = (TH1D*)f.Get("Lgap");
  // hist4->Draw("HIST");
}
