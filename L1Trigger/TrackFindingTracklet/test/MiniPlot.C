// -----------------------------------------------------------------------------
// Basic example ROOT script for making quick plots of track distributions using
// the ntuples produced by L1TrackNtupleMaker.cc
//
// e.g. in ROOT do: .L MiniPlot.C++, MiniPlot("TTbar_PU200_D49")
//
// By Louise Skinnari
// Edited by Emily Tsai
// -----------------------------------------------------------------------------

#include "TROOT.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TMath.h"
#include <TError.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

float PI = 3.14159265358979323846;

using namespace std;

void SetPlotStyle();
void mySmallText(Double_t x, Double_t y, Color_t color, char* text);


// -----------------------------------------------------------------------------
// Main script
// -----------------------------------------------------------------------------

// type: Name of the input file you want to process (Minus ".root" extension)
void MiniPlot(TString type, TString type_dir = "")
{
	gROOT->SetBatch();
	gErrorIgnoreLevel = kWarning;

	SetPlotStyle();


	// -------------------------------------------------------------------------
	// Read ntuples
	// -------------------------------------------------------------------------

	TChain* tree = new TChain("L1TrackNtuple/eventTree");
	tree->Add(type_dir + type + ".root");

	if(tree->GetEntries() == 0)
	{
		cout << "File doesn't exist or is empty, returning..." << endl;
		return;
	}


	// -------------------------------------------------------------------------
	// Define leafs & branches
	// -------------------------------------------------------------------------

	// Tracking particles ("true")
	vector<float>* tp_pt;
	vector<float>* tp_eta;
	vector<float>* tp_phi;
	vector<float>* tp_dxy;
	vector<float>* tp_z0;
	vector<float>* tp_d0;
	vector<int>* tp_pdgid;
	vector<int>* tp_nmatch;
	vector<int>* tp_nstub;
	vector<int>* tp_eventid;

	// *L1 track* properties, for tracking particles matched to a L1 track
	vector<float>* matchtrk_pt;
	vector<float>* matchtrk_eta;
	vector<float>* matchtrk_phi;
	vector<float>* matchtrk_z0;
	vector<float>* matchtrk_d0;
	vector<float>* matchtrk_chi2;
	vector<float>* matchtrk_chi2rphi;
	vector<float>* matchtrk_chi2rz;
	vector<int>* matchtrk_nstub;
	vector<int>* matchtrk_lhits;
	vector<int>* matchtrk_dhits;
	vector<int>* matchtrk_seed;
	vector<int>* matchtrk_hitpattern;

	// All L1 tracks (algorithm)
	vector<float>* trk_pt;
	vector<float>* trk_eta;
	vector<float>* trk_phi;
	vector<float>* trk_z0;
	vector<float>* trk_d0;
	vector<float>* trk_chi2;
	vector<float>* trk_chi2rphi;
	vector<float>* trk_chi2rz;
	vector<int>* trk_nstub;
	vector<int>* trk_lhits;
	vector<int>* trk_dhits;
	vector<int>* trk_seed;
	vector<int>* trk_hitpattern;
	vector<unsigned int>* trk_phiSector;
	vector<int>* trk_fake;
	vector<int>* trk_genuine;
	vector<int>* trk_loose;

	TBranch* b_tp_pt;
	TBranch* b_tp_eta;
	TBranch* b_tp_phi;
	TBranch* b_tp_dxy;
	TBranch* b_tp_z0;
	TBranch* b_tp_d0;
	TBranch* b_tp_pdgid;
	TBranch* b_tp_nmatch;
	TBranch* b_tp_nstub;
	TBranch* b_tp_eventid;

	TBranch* b_matchtrk_pt;
	TBranch* b_matchtrk_eta;
	TBranch* b_matchtrk_phi;
	TBranch* b_matchtrk_z0;
	TBranch* b_matchtrk_d0;
	TBranch* b_matchtrk_chi2;
	TBranch* b_matchtrk_chi2rphi;
	TBranch* b_matchtrk_chi2rz;
	TBranch* b_matchtrk_nstub;
	TBranch* b_matchtrk_lhits;
	TBranch* b_matchtrk_dhits;
	TBranch* b_matchtrk_seed;
	TBranch* b_matchtrk_hitpattern;

	TBranch* b_trk_pt;
	TBranch* b_trk_eta;
	TBranch* b_trk_phi;
	TBranch* b_trk_z0;
	TBranch* b_trk_d0;
	TBranch* b_trk_chi2;
	TBranch* b_trk_chi2rphi;
	TBranch* b_trk_chi2rz;
	TBranch* b_trk_nstub;
	TBranch* b_trk_lhits;
	TBranch* b_trk_dhits;
	TBranch* b_trk_phiSector;
	TBranch* b_trk_seed;
	TBranch* b_trk_hitpattern;
	TBranch* b_trk_fake;
	TBranch* b_trk_genuine;
	TBranch* b_trk_loose;

	tp_pt = 0;
	tp_eta = 0;
	tp_phi = 0;
	tp_dxy = 0;
	tp_z0 = 0;
	tp_d0 = 0;
	tp_pdgid = 0;
	tp_nmatch = 0;
	tp_nstub = 0;
	tp_eventid = 0;

	matchtrk_pt = 0;
	matchtrk_eta = 0;
	matchtrk_phi = 0;
	matchtrk_z0 = 0;
	matchtrk_d0 = 0;
	matchtrk_chi2 = 0;
	matchtrk_chi2rphi = 0;
	matchtrk_chi2rz = 0;
	matchtrk_nstub = 0;
	matchtrk_lhits = 0;
	matchtrk_dhits = 0;
	matchtrk_seed = 0;
	matchtrk_hitpattern = 0;

	trk_pt = 0;
	trk_eta = 0;
	trk_phi = 0;
	trk_z0 = 0;
	trk_d0 = 0;
	trk_chi2 = 0;
	trk_chi2rphi = 0;
	trk_chi2rz = 0;
	trk_nstub = 0;
	trk_lhits = 0;
	trk_dhits = 0;
	trk_phiSector = 0;
	trk_seed = 0;
	trk_hitpattern = 0;
	trk_fake = 0;
	trk_genuine = 0;
	trk_loose = 0;

	tree->SetBranchAddress("tp_pt", &tp_pt, &b_tp_pt);
	tree->SetBranchAddress("tp_eta", &tp_eta, &b_tp_eta);
	tree->SetBranchAddress("tp_phi", &tp_phi, &b_tp_phi);
	tree->SetBranchAddress("tp_dxy", &tp_dxy, &b_tp_dxy);
	tree->SetBranchAddress("tp_z0", &tp_z0, &b_tp_z0);
	tree->SetBranchAddress("tp_d0", &tp_d0, &b_tp_d0);
	tree->SetBranchAddress("tp_pdgid", &tp_pdgid, &b_tp_pdgid);
	tree->SetBranchAddress("tp_nmatch", &tp_nmatch, &b_tp_nmatch);
	tree->SetBranchAddress("tp_nstub", &tp_nstub, &b_tp_nstub);
	tree->SetBranchAddress("tp_eventid", &tp_eventid, &b_tp_eventid);

	tree->SetBranchAddress("matchtrk_pt", &matchtrk_pt, &b_matchtrk_pt);
	tree->SetBranchAddress("matchtrk_eta", &matchtrk_eta, &b_matchtrk_eta);
	tree->SetBranchAddress("matchtrk_phi", &matchtrk_phi, &b_matchtrk_phi);
	tree->SetBranchAddress("matchtrk_z0", &matchtrk_z0, &b_matchtrk_z0);
	tree->SetBranchAddress("matchtrk_d0", &matchtrk_d0, &b_matchtrk_d0);
	tree->SetBranchAddress("matchtrk_chi2", &matchtrk_chi2, &b_matchtrk_chi2);
	tree->SetBranchAddress("matchtrk_chi2rphi", &matchtrk_chi2rphi, &b_matchtrk_chi2rphi);
	tree->SetBranchAddress("matchtrk_chi2rz", &matchtrk_chi2rz, &b_matchtrk_chi2rz);
	tree->SetBranchAddress("matchtrk_nstub", &matchtrk_nstub, &b_matchtrk_nstub);
	tree->SetBranchAddress("matchtrk_lhits", &matchtrk_lhits, &b_matchtrk_lhits);
	tree->SetBranchAddress("matchtrk_dhits", &matchtrk_dhits, &b_matchtrk_dhits);
	tree->SetBranchAddress("matchtrk_seed", &matchtrk_seed, &b_matchtrk_seed);
	tree->SetBranchAddress("matchtrk_hitpattern", &matchtrk_hitpattern, &b_matchtrk_hitpattern);

	tree->SetBranchAddress("trk_pt", &trk_pt, &b_trk_pt);
	tree->SetBranchAddress("trk_eta", &trk_eta, &b_trk_eta);
	tree->SetBranchAddress("trk_phi", &trk_phi, &b_trk_phi);
	tree->SetBranchAddress("trk_z0", &trk_z0, &b_trk_z0);
	tree->SetBranchAddress("trk_d0", &trk_d0, &b_trk_d0);
	tree->SetBranchAddress("trk_chi2", &trk_chi2, &b_trk_chi2);
	tree->SetBranchAddress("trk_chi2rphi", &trk_chi2rphi, &b_trk_chi2rphi);
	tree->SetBranchAddress("trk_chi2rz", &trk_chi2rz, &b_trk_chi2rz);
	tree->SetBranchAddress("trk_nstub", &trk_nstub, &b_trk_nstub);
	tree->SetBranchAddress("trk_lhits", &trk_lhits, &b_trk_lhits);
	tree->SetBranchAddress("trk_dhits", &trk_dhits, &b_trk_dhits);
	tree->SetBranchAddress("trk_phiSector", &trk_phiSector, &b_trk_phiSector);
	tree->SetBranchAddress("trk_seed", &trk_seed, &b_trk_seed);
	tree->SetBranchAddress("trk_hitpattern", &trk_hitpattern, &b_trk_hitpattern);
	tree->SetBranchAddress("trk_fake", &trk_fake, &b_trk_fake);
	tree->SetBranchAddress("trk_genuine", &trk_genuine, &b_trk_genuine);
	tree->SetBranchAddress("trk_loose", &trk_loose, &b_trk_loose);


	// -------------------------------------------------------------------------
	// Histograms
	// -------------------------------------------------------------------------

	/////////////////////////////////////////////////////
    // NOTATION:                                       //
    // 'C' - Central eta range, |eta| < 0.8            //
    // 'I' - Intermediate eta range, 0.8 < |eta| < 1.6 //
    // 'F' - Forward eta range, |eta| > 1.6            //
    //                                                 //
    // 'L' - Low pt range,  pt < 8 GeV                 //
    // 'H' - High pt range, pt > 8 GeV                 //
    /////////////////////////////////////////////////////

	TH1F* h_trk_pt = new TH1F("trk_pt", ";Track p_{T} (GeV); Tracks / 1.0 GeV", 100, 0, 100.0);
	TH1F* h_tp_pt = new TH1F("tp_pt", ";Tracking particle p_{T} (GeV); Tracking particles / 1.0 GeV", 100, 0, 100.0);

	// Here you can create various histograms
	// TODO


	// -------------------------------------------------------------------------
	//       * * * * *     START OF ACTUAL RUNNING ON EVENTS     * * * * *
	// -------------------------------------------------------------------------

	int nevt = tree->GetEntries();
	cout << "number of events = " << nevt << endl;


	// -------------------------------------------------------------------------
	// Event loop
	// -------------------------------------------------------------------------

	for(int i = 0; i < nevt; i++)
	{
		tree->GetEntry(i, 0);


		// ---------------------------------------------------------------------
		// Loop over *all* found found L1 tracks
		// ---------------------------------------------------------------------

		for(int it = 0; it < (int) trk_pt->size(); it++)
		{
			// Here you can place some selection criteria and fill histograms
			// for the L1 tracks
			// TODO

			// Fill histogram "h_trk_pt" with the track pt for each track in the
			// event
			h_trk_pt->Fill(trk_pt->at(it));

			// Here you can analyze tracks and fill histograms
			// TODO
		}


		// ---------------------------------------------------------------------
		// Loop over all "tracking particles" ("true" tracks)
		// ---------------------------------------------------------------------

		for(int it = 0; it < (int) tp_pt->size(); it++)
		{
			// Here you can place some selection criteria and fill histograms
			// for the tracking particles
			// TODO

			// If tracking particle pt < 2 GeV, we continue to the next one
			if(tp_pt->at(it) < 2.0) continue;

			h_tp_pt->Fill(tp_pt->at(it));

			// -----------------------------------------------------------------

			// Was the tracking particle matched to a L1 track?
			if(tp_nmatch->at(it) < 1) continue;

			// Here you have tracking particles for which an L1 track was
			// successfully identified, and could fill histograms for those
			// TODO
		}
	}


	// -------------------------------------------------------------------------
	// Output file for histograms
	// -------------------------------------------------------------------------

	TFile* fout = new TFile(type_dir + "miniout_" + type + ".root", "recreate");


	// -------------------------------------------------------------------------
	// Draw and save plots
	// -------------------------------------------------------------------------

	char ctxt[500];
	TCanvas c;

	gSystem->mkdir("output/TrackPlots");
	TString DIR = "output/TrackPlots/";

	h_trk_pt->SetMinimum(0);
	h_trk_pt->Draw();

	// Example for how to put some text on the canvas
	// sprintf(ctxt, "|eta| < 2.4");
	// mySmallText(0.52, 0.82, 1, ctxt);

	// Save histogram as pdf
	c.SaveAs(DIR + type + "_trk_pt.pdf");

	// Save histogram to a small output ROOT file
	h_trk_pt->Write();

	h_tp_pt->SetMinimum(0);
	h_tp_pt->Draw();

	c.SaveAs(DIR + type + "_tp_pt.pdf");

	h_tp_pt->Write();

	// Close output ROOT file
	fout->Close();
}


void SetPlotStyle()
{
	// Use plain black on white colors
	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameFillColor(0);
	gStyle->SetCanvasBorderMode(0);
	gStyle->SetCanvasColor(0);
	gStyle->SetPadBorderMode(0);
	gStyle->SetPadColor(0);
	gStyle->SetStatColor(0);
	gStyle->SetHistLineColor(1);

	gStyle->SetPalette(1);

	// Set the paper & margin sizes
	gStyle->SetPaperSize(20, 26);
	gStyle->SetPadTopMargin(0.05);
	gStyle->SetPadRightMargin(0.05);
	gStyle->SetPadBottomMargin(0.16);
	gStyle->SetPadLeftMargin(0.16);

	// Set title offsets (for axis label)
	gStyle->SetTitleXOffset(1.4);
	gStyle->SetTitleYOffset(1.4);

	// Use large fonts
	gStyle->SetTextFont(42);
	gStyle->SetTextSize(0.05);
	gStyle->SetLabelFont(42, "x");
	gStyle->SetTitleFont(42, "x");
	gStyle->SetLabelFont(42, "y");
	gStyle->SetTitleFont(42, "y");
	gStyle->SetLabelFont(42, "z");
	gStyle->SetTitleFont(42, "z");
	gStyle->SetLabelSize(0.05, "x");
	gStyle->SetTitleSize(0.05, "x");
	gStyle->SetLabelSize(0.05, "y");
	gStyle->SetTitleSize(0.05, "y");
	gStyle->SetLabelSize(0.05, "z");
	gStyle->SetTitleSize(0.05, "z");

	// Use bold lines and markers
	gStyle->SetMarkerStyle(20);
	gStyle->SetMarkerSize(1.2);
	gStyle->SetHistLineWidth(2.);
	gStyle->SetLineStyleString(2, "[12 12]");

	// Get rid of error bar caps
	gStyle->SetEndErrorSize(0.);

	// Do not display any of the standard histogram decorations
	gStyle->SetOptTitle(0);
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);

	// Put tick marks on top and RHS of plots
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
}


void mySmallText(Double_t x, Double_t y, Color_t color, char* text)
{
	Double_t tsize = 0.044;
	TLatex l;
	l.SetTextSize(tsize);
	l.SetNDC();
	l.SetTextColor(color);
	l.DrawLatex(x, y, text);
}
