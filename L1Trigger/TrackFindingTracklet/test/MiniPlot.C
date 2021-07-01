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
#include <map>

float PI = 3.14159265358979323846;
float MHZ = 30.0;
float KHZ = MHZ * 1000.0;

float TRK_PT_OVERFLOW = 100.0;
int TRK_NSTUB_OVERFLOW = 10;
float TRK_CHI2_OVERFLOW = 100.0;
float TRK_CHI2DOF_OVERFLOW = 20.0;
float TRK_BENDCHI2_OVERFLOW = 20.0;
double BUFFER = 0.1;

int N_PT_CUTS = 11;
float PT_CUTS[] = {0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0};
float PT_MIN_BIN = -5.0;
float PT_MAX_BIN = 105.0;

int N_CHI2DOF_CUTS = 4;
float CHI2DOF_CUTS[] = {2.0, 3.0, 4.0, 5.0};
int N_BENDCHI2_CUTS = 4;
float BENDCHI2_CUTS[] = {2.0, 2.5, 3.0, 3.5};

using namespace std;

void SetPlotStyle();
void mySmallText(Double_t x, Double_t y, Color_t color, char* text);
void compare4Plot(TString dir, TString type, TString var,
	TH1F* h, TString hlabel,
	TH1F* h1, TString h1label,
	TH1F* h2, TString h2label,
	TH1F* h3, TString h3label,
	TH1F* h4, TString h4label);


// -----------------------------------------------------------------------------
// Main script
// -----------------------------------------------------------------------------

// type: Name of the input file you want to process (Minus ".root" extension)
void MiniPlot(TString type, TString type_dir = "")
{
	gROOT->SetBatch();
	gErrorIgnoreLevel = kWarning;

	SetPlotStyle();

	bool print = 1;


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
	vector<float>* tp_d0;
	vector<float>* tp_z0;
	vector<float>* tp_d0_prod;
	vector<float>* tp_z0_prod;
	vector<int>* tp_pdgid;
	vector<int>* tp_nmatch;
	vector<int>* tp_nstub;
	vector<int>* tp_eventid;
	vector<int>* tp_charge;

	// *L1 track* properties, for tracking particles matched to a L1 track
	vector<float>* matchtrk_pt;
	vector<float>* matchtrk_eta;
	vector<float>* matchtrk_phi;
	vector<float>* matchtrk_z0;
	vector<float>* matchtrk_d0;
	vector<float>* matchtrk_chi2;
	vector<float>* matchtrk_chi2rphi;
	vector<float>* matchtrk_chi2rz;
	vector<float>* matchtrk_bendchi2;
	vector<float>* matchtrk_MVA1;
	vector<int>* matchtrk_nstub;
	vector<int>* matchtrk_lhits;
	vector<int>* matchtrk_dhits;
	vector<int>* matchtrk_seed;
	vector<int>* matchtrk_hitpattern;

	// All L1 tracks (algorithm)
	vector<float>* trk_pt;
	vector<float>* trk_eta;
	vector<float>* trk_phi;
	vector<float>* trk_d0;
	vector<float>* trk_z0;
	vector<float>* trk_chi2;
	vector<float>* trk_chi2rphi;
	vector<float>* trk_chi2rz;
	vector<float>* trk_bendchi2;
	vector<int>* trk_nstub;
	vector<int>* trk_lhits;
	vector<int>* trk_dhits;
	vector<int>* trk_seed;
	vector<int>* trk_hitpattern;
	vector<unsigned int>* trk_phiSector;
	vector<int>* trk_genuine;
	vector<int>* trk_loose;
	vector<int>* trk_unknown;
	vector<int>* trk_combinatoric;
	vector<int>* trk_fake;
	vector<int>* trk_MVA1;
	vector<int>* trk_matchtp_pdgid;
	vector<int>* trk_matchtp_pt;
	vector<int>* trk_matchtp_eta;
	vector<int>* trk_matchtp_phi;
	vector<int>* trk_matchtp_z0;
	vector<int>* trk_matchtp_dxy;
	vector<int>* trk_matchtp_d0;

	TBranch* b_tp_pt;
	TBranch* b_tp_eta;
	TBranch* b_tp_phi;
	TBranch* b_tp_dxy;
	TBranch* b_tp_d0;
	TBranch* b_tp_z0;
	TBranch* b_tp_d0_prod;
	TBranch* b_tp_z0_prod;
	TBranch* b_tp_pdgid;
	TBranch* b_tp_nmatch;
	TBranch* b_tp_nstub;
	TBranch* b_tp_eventid;
	TBranch* b_tp_charge;

	TBranch* b_matchtrk_pt;
	TBranch* b_matchtrk_eta;
	TBranch* b_matchtrk_phi;
	TBranch* b_matchtrk_z0;
	TBranch* b_matchtrk_d0;
	TBranch* b_matchtrk_chi2;
	TBranch* b_matchtrk_chi2rphi;
	TBranch* b_matchtrk_chi2rz;
	TBranch* b_matchtrk_bendchi2;
	TBranch* b_matchtrk_MVA1;
	TBranch* b_matchtrk_nstub;
	TBranch* b_matchtrk_lhits;
	TBranch* b_matchtrk_dhits;
	TBranch* b_matchtrk_seed;
	TBranch* b_matchtrk_hitpattern;

	TBranch* b_trk_pt;
	TBranch* b_trk_eta;
	TBranch* b_trk_phi;
	TBranch* b_trk_d0;
	TBranch* b_trk_z0;
	TBranch* b_trk_chi2;
	TBranch* b_trk_chi2rphi;
	TBranch* b_trk_chi2rz;
	TBranch* b_trk_bendchi2;
	TBranch* b_trk_nstub;
	TBranch* b_trk_lhits;
	TBranch* b_trk_dhits;
	TBranch* b_trk_seed;
	TBranch* b_trk_hitpattern;
	TBranch* b_trk_phiSector;
	TBranch* b_trk_genuine;
	TBranch* b_trk_loose;
	TBranch* b_trk_unknown;
	TBranch* b_trk_combinatoric;
	TBranch* b_trk_fake;
	TBranch* b_trk_MVA1;
	TBranch* b_trk_matchtp_pdgid;
	TBranch* b_trk_matchtp_pt;
	TBranch* b_trk_matchtp_eta;
	TBranch* b_trk_matchtp_phi;
	TBranch* b_trk_matchtp_z0;
	TBranch* b_trk_matchtp_dxy;
	TBranch* b_trk_matchtp_d0;

	tp_pt = 0;
	tp_eta = 0;
	tp_phi = 0;
	tp_dxy = 0;
	tp_d0 = 0;
	tp_z0 = 0;
	tp_d0_prod = 0;
	tp_z0_prod = 0;
	tp_pdgid = 0;
	tp_nmatch = 0;
	tp_nstub = 0;
	tp_eventid = 0;
	tp_charge = 0;

	matchtrk_pt = 0;
	matchtrk_eta = 0;
	matchtrk_phi = 0;
	matchtrk_z0 = 0;
	matchtrk_d0 = 0;
	matchtrk_chi2 = 0;
	matchtrk_chi2rphi = 0;
	matchtrk_chi2rz = 0;
	matchtrk_bendchi2 = 0;
	matchtrk_MVA1 = 0;
	matchtrk_nstub = 0;
	matchtrk_lhits = 0;
	matchtrk_dhits = 0;
	matchtrk_seed = 0;
	matchtrk_hitpattern = 0;

	trk_pt = 0;
	trk_eta = 0;
	trk_phi = 0;
	trk_d0 = 0;
	trk_z0 = 0;
	trk_chi2 = 0;
	trk_chi2rphi = 0;
	trk_chi2rz = 0;
	trk_bendchi2 = 0;
	trk_nstub = 0;
	trk_lhits = 0;
	trk_dhits = 0;
	trk_seed = 0;
	trk_hitpattern = 0;
	trk_phiSector = 0;
	trk_genuine = 0;
	trk_loose = 0;
	trk_unknown = 0;
	trk_combinatoric = 0;
	trk_fake = 0;
	trk_MVA1 = 0;
	trk_matchtp_pdgid = 0;
	trk_matchtp_pt = 0;
	trk_matchtp_eta = 0;
	trk_matchtp_phi = 0;
	trk_matchtp_z0 = 0;
	trk_matchtp_dxy = 0;
	trk_matchtp_d0 = 0;

	tree->SetBranchAddress("tp_pt", &tp_pt, &b_tp_pt);
	tree->SetBranchAddress("tp_eta", &tp_eta, &b_tp_eta);
	tree->SetBranchAddress("tp_phi", &tp_phi, &b_tp_phi);
	tree->SetBranchAddress("tp_dxy", &tp_dxy, &b_tp_dxy);
	tree->SetBranchAddress("tp_d0", &tp_d0, &b_tp_d0);
	tree->SetBranchAddress("tp_z0", &tp_z0, &b_tp_z0);
	tree->SetBranchAddress("tp_d0_prod", &tp_d0_prod, &b_tp_d0_prod);
	tree->SetBranchAddress("tp_z0_prod", &tp_z0_prod, &b_tp_z0_prod);
	tree->SetBranchAddress("tp_pdgid", &tp_pdgid, &b_tp_pdgid);
	tree->SetBranchAddress("tp_nmatch", &tp_nmatch, &b_tp_nmatch);
	tree->SetBranchAddress("tp_nstub", &tp_nstub, &b_tp_nstub);
	tree->SetBranchAddress("tp_eventid", &tp_eventid, &b_tp_eventid);
	tree->SetBranchAddress("tp_charge", &tp_charge, &b_tp_charge);

	tree->SetBranchAddress("matchtrk_pt", &matchtrk_pt, &b_matchtrk_pt);
	tree->SetBranchAddress("matchtrk_eta", &matchtrk_eta, &b_matchtrk_eta);
	tree->SetBranchAddress("matchtrk_phi", &matchtrk_phi, &b_matchtrk_phi);
	tree->SetBranchAddress("matchtrk_z0", &matchtrk_z0, &b_matchtrk_z0);
	tree->SetBranchAddress("matchtrk_d0", &matchtrk_d0, &b_matchtrk_d0);
	tree->SetBranchAddress("matchtrk_chi2", &matchtrk_chi2, &b_matchtrk_chi2);
	tree->SetBranchAddress("matchtrk_chi2rphi", &matchtrk_chi2rphi, &b_matchtrk_chi2rphi);
	tree->SetBranchAddress("matchtrk_chi2rz", &matchtrk_chi2rz, &b_matchtrk_chi2rz);
	tree->SetBranchAddress("matchtrk_bendchi2", &matchtrk_bendchi2, &b_matchtrk_bendchi2);
	tree->SetBranchAddress("matchtrk_MVA1", &matchtrk_MVA1, &b_matchtrk_MVA1);
	tree->SetBranchAddress("matchtrk_nstub", &matchtrk_nstub, &b_matchtrk_nstub);
	tree->SetBranchAddress("matchtrk_lhits", &matchtrk_lhits, &b_matchtrk_lhits);
	tree->SetBranchAddress("matchtrk_dhits", &matchtrk_dhits, &b_matchtrk_dhits);
	tree->SetBranchAddress("matchtrk_seed", &matchtrk_seed, &b_matchtrk_seed);
	tree->SetBranchAddress("matchtrk_hitpattern", &matchtrk_hitpattern, &b_matchtrk_hitpattern);

	tree->SetBranchAddress("trk_pt", &trk_pt, &b_trk_pt);
	tree->SetBranchAddress("trk_eta", &trk_eta, &b_trk_eta);
	tree->SetBranchAddress("trk_phi", &trk_phi, &b_trk_phi);
	tree->SetBranchAddress("trk_d0", &trk_d0, &b_trk_d0);
	tree->SetBranchAddress("trk_z0", &trk_z0, &b_trk_z0);
	tree->SetBranchAddress("trk_chi2", &trk_chi2, &b_trk_chi2);
	tree->SetBranchAddress("trk_chi2rphi", &trk_chi2rphi, &b_trk_chi2rphi);
	tree->SetBranchAddress("trk_chi2rz", &trk_chi2rz, &b_trk_chi2rz);
	tree->SetBranchAddress("trk_bendchi2", &trk_bendchi2, &b_trk_bendchi2);
	tree->SetBranchAddress("trk_nstub", &trk_nstub, &b_trk_nstub);
	tree->SetBranchAddress("trk_lhits", &trk_lhits, &b_trk_lhits);
	tree->SetBranchAddress("trk_dhits", &trk_dhits, &b_trk_dhits);
	tree->SetBranchAddress("trk_seed", &trk_seed, &b_trk_seed);
	tree->SetBranchAddress("trk_hitpattern", &trk_hitpattern, &b_trk_hitpattern);
	tree->SetBranchAddress("trk_phiSector", &trk_phiSector, &b_trk_phiSector);
	tree->SetBranchAddress("trk_genuine", &trk_genuine, &b_trk_genuine);
	tree->SetBranchAddress("trk_loose", &trk_loose, &b_trk_loose);
	tree->SetBranchAddress("trk_unknown", &trk_unknown, &b_trk_unknown);
	tree->SetBranchAddress("trk_combinatoric", &trk_combinatoric, &b_trk_combinatoric);
	tree->SetBranchAddress("trk_fake", &trk_fake, &b_trk_fake);
	tree->SetBranchAddress("trk_MVA1", &trk_MVA1, &b_trk_MVA1);
	tree->SetBranchAddress("trk_matchtp_pdgid", &trk_matchtp_pdgid, &b_trk_matchtp_pdgid);
	tree->SetBranchAddress("trk_matchtp_pt", &trk_matchtp_pt, &b_trk_matchtp_pt);
	tree->SetBranchAddress("trk_matchtp_eta", &trk_matchtp_eta, &b_trk_matchtp_eta);
	tree->SetBranchAddress("trk_matchtp_phi", &trk_matchtp_phi, &b_trk_matchtp_phi);
	tree->SetBranchAddress("trk_matchtp_z0", &trk_matchtp_z0, &b_trk_matchtp_z0);
	tree->SetBranchAddress("trk_matchtp_dxy", &trk_matchtp_dxy, &b_trk_matchtp_dxy);
	tree->SetBranchAddress("trk_matchtp_d0", &trk_matchtp_d0, &b_trk_matchtp_d0);


	// -------------------------------------------------------------------------
	// Histograms
	// -------------------------------------------------------------------------

	// Hold histograms so you can loop through them
	map<TString, TH1F*> h;

	// -----------------------------------------------------

	h["trk_pt"] = new TH1F("trk_pt",
		"; Track p_{T} [GeV]; Tracks / 1.0 GeV",
		100, 0, 100.0);

	h["trk_nstub"] = new TH1F("trk_nstub",
		"; Number of stubs; Tracks / 1.0",
		10, 0, 10);

	h["trk_chi2"] = new TH1F("trk_chi2",
		"; #chi^{2}; Tracks / 1.0",
		100, 0, 100.0);

	h["trk_chi2dof"] = new TH1F("trk_chi2dof",
		"; #chi^{2} / D.O.F.; Tracks / 0.2",
		100, 0, 20.0);

	h["trk_bendchi2"] = new TH1F("trk_bendchi2",
		"; #chi^{2}_{bend}; Tracks / 1.0",
		100, 0, 20.0);

	// -----------------------------------------------------

	h["tp_pt"] = new TH1F("tp_pt",
		"; Tracking particle p_{T} [GeV]; Tracking particles / 1.0 GeV",
		100, 0, 100.0);

	// -----------------------------------------------------

	/////////////////////////////////////////////////////
    // NOTATION:                                       //
    // 'g' -> greater than                             //
    /////////////////////////////////////////////////////

	h["nevt"] = new TH1F("nevt",
		"; Track p_{T} [GeV]; Rate [kHz]",
		N_PT_CUTS, PT_MIN_BIN, PT_MAX_BIN);

	for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
	{
		TString name = "nevt_";
		name += CHI2DOF_CUTS[cd];
		name += "chi2dof";

		h[name] = new TH1F(name,
			"; Track p_{T} [GeV]; Rate [kHz]",
			N_PT_CUTS, PT_MIN_BIN, PT_MAX_BIN);
	}

	for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
	{
		TString name = "nevt_";
		name += BENDCHI2_CUTS[bc];
		name += "bendchi2";

		h[name] = new TH1F(name,
			"; Track p_{T} [GeV]; Rate [kHz]",
			N_PT_CUTS, PT_MIN_BIN, PT_MAX_BIN);
	}

	for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
	{
		for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
		{
			TString name = "nevt_";
			name += CHI2DOF_CUTS[cd];
			name += "chi2dof_";
			name += BENDCHI2_CUTS[bc];
			name += "bendchi2";

			h[name] = new TH1F(name,
				"; Track p_{T} [GeV]; Rate [kHz]",
				N_PT_CUTS, PT_MIN_BIN, PT_MAX_BIN);
		}
	}

	// -----------------------------------------------------

	// Here you can create various histograms
	// TODO


	// -------------------------------------------------------------------------
	//       * * * * *     START OF ACTUAL RUNNING ON EVENTS     * * * * *
	// -------------------------------------------------------------------------

	int nevt = tree->GetEntries();
	cout << "number of events = " << nevt << endl;

	// Counters for number of events
	map<TString, vector<int>> n;

	n["nevt"] = vector<int>(N_PT_CUTS);

	for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
	{
		TString name = "nevt_";
		name += CHI2DOF_CUTS[cd];
		name += "chi2dof";
		n[name] = vector<int>(N_PT_CUTS);
	}

	for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
	{
		TString name = "nevt_";
		name += BENDCHI2_CUTS[bc];
		name += "bendchi2";
		n[name] = vector<int>(N_PT_CUTS);
	}

	for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
	{
		for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
		{
			TString name = "nevt_";
			name += CHI2DOF_CUTS[cd];
			name += "chi2dof_";
			name += BENDCHI2_CUTS[bc];
			name += "bendchi2";
			n[name] = vector<int>(N_PT_CUTS);
		}
	}


	// -------------------------------------------------------------------------
	// Event loop
	// -------------------------------------------------------------------------

	for(int i = 0; i < nevt; i++)
	{
		tree->GetEntry(i, 0);

		// Store if event has already been added
		map<TString, vector<bool>> in;

		in["nevt"] = vector<bool>(N_PT_CUTS);

		for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
		{
			TString name = "nevt_";
			name += CHI2DOF_CUTS[cd];
			name += "chi2dof";
			in[name] = vector<bool>(N_PT_CUTS);
		}

		for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
		{
			TString name = "nevt_";
			name += BENDCHI2_CUTS[bc];
			name += "bendchi2";
			in[name] = vector<bool>(N_PT_CUTS);
		}

		for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
		{
			for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
			{
				TString name = "nevt_";
				name += CHI2DOF_CUTS[cd];
				name += "chi2dof_";
				name += BENDCHI2_CUTS[bc];
				name += "bendchi2";
				in[name] = vector<bool>(N_PT_CUTS);
			}
		}


		// ---------------------------------------------------------------------
		// Loop over *all* found L1 tracks
		// ---------------------------------------------------------------------

		for(int it = 0; it < (int) trk_pt->size(); it++)
		{
			float pt = trk_pt->at(it);
			int n_stub = trk_nstub->at(it);
			int n_dof = 2 * n_stub - 4;
			float chi2 = trk_chi2->at(it);
			float chi2dof = chi2 / (float) n_dof;
			float bendchi2 = trk_bendchi2->at(it);

			if(std::abs(trk_eta->at(it)) > 2.4) continue;
			if(pt < 2.0) continue;

			if(pt > TRK_PT_OVERFLOW)
				h["trk_pt"]->Fill(TRK_PT_OVERFLOW - BUFFER);
			else h["trk_pt"]->Fill(pt);

			if(n_stub > TRK_NSTUB_OVERFLOW)
				h["trk_nstub"]->Fill(TRK_NSTUB_OVERFLOW - BUFFER);
			else h["trk_nstub"]->Fill(n_stub);

			if(chi2 > TRK_CHI2_OVERFLOW)
				h["trk_chi2"]->Fill(TRK_CHI2_OVERFLOW - BUFFER);
			else h["trk_chi2"]->Fill(chi2);

			if(chi2dof > TRK_CHI2DOF_OVERFLOW)
				h["trk_chi2dof"]->Fill(TRK_CHI2DOF_OVERFLOW - BUFFER);
			else h["trk_chi2dof"]->Fill(chi2dof);

			if(bendchi2 > TRK_BENDCHI2_OVERFLOW)
				h["trk_bendchi2"]->Fill(TRK_BENDCHI2_OVERFLOW - BUFFER);
			else h["trk_bendchi2"]->Fill(bendchi2);

			for(int cut = 0; cut < N_PT_CUTS; cut++)
			{
				if(pt > PT_CUTS[cut])
				{
					if(!in["nevt"][cut])
					{
						n["nevt"][cut]++;
						in["nevt"][cut] = true;
					}

					for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
					{
						TString name = "nevt_";
						name += CHI2DOF_CUTS[cd];
						name += "chi2dof";

						if(!in[name][cut]
							&& chi2dof < CHI2DOF_CUTS[cd])
						{
							n[name][cut]++;
							in[name][cut] = true;
						}
					}

					for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
					{
						TString name = "nevt_";
						name += BENDCHI2_CUTS[bc];
						name += "bendchi2";

						if(!in[name][cut]
							&& bendchi2 < BENDCHI2_CUTS[bc])
						{
							n[name][cut]++;
							in[name][cut] = true;
						}
					}

					for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
					{
						for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
						{
							TString name = "nevt_";
							name += CHI2DOF_CUTS[cd];
							name += "chi2dof_";
							name += BENDCHI2_CUTS[bc];
							name += "bendchi2";

							if(!in[name][cut]
								&& chi2dof < CHI2DOF_CUTS[cd]
								&& bendchi2 < BENDCHI2_CUTS[bc])
							{
								n[name][cut]++;
								in[name][cut] = true;
							}
						}
					}
				}
			}

			// Here you can place some selection criteria and fill histograms
			// for the L1 tracks
			// TODO
		}

		// Here you can analyze tracks and fill histograms
		// TODO


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

			// Fill histogram "h_tp_pt" with track pt for each track in event
			h["tp_pt"]->Fill(tp_pt->at(it));

			// -----------------------------------------------------------------

			// Was the tracking particle matched to a L1 track?
			// if(tp_nmatch->at(it) < 1) continue;

			// Here you have tracking particles for which an L1 track was
			// successfully identified, and could fill histograms for those
			// TODO
		}
	}

	// for(int pt_cut = MIN_PT; pt_cut <= MAX_PT; pt_cut += INC_PT)
	for(int cut = 0; cut < N_PT_CUTS; cut++)
	{
		h["nevt"]->SetBinContent(cut + 1,
			(float) n["nevt"][cut] / nevt * KHZ);
		h["nevt"]->SetBinError(cut + 1,
			sqrt((float) n["nevt"][cut]) / nevt * KHZ);
	}

	for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
	{
		TString name = "nevt_";
		name += CHI2DOF_CUTS[cd];
		name += "chi2dof";

		for(int cut = 0; cut < N_PT_CUTS; cut++)
		{
			h[name]->SetBinContent(cut + 1,
				(float) n[name][cut] / nevt * KHZ);
			h[name]->SetBinError(cut + 1,
				sqrt((float) n[name][cut]) / nevt * KHZ);
		}
	}

	for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
	{
		TString name = "nevt_";
		name += BENDCHI2_CUTS[bc];
		name += "bendchi2";

		for(int cut = 0; cut < N_PT_CUTS; cut++)
		{
			h[name]->SetBinContent(cut + 1,
				(float) n[name][cut] / nevt * KHZ);
			h[name]->SetBinError(cut + 1,
				sqrt((float) n[name][cut]) / nevt * KHZ);
		}
	}

	for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
	{
		for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
		{
			TString name = "nevt_";
			name += CHI2DOF_CUTS[cd];
			name += "chi2dof_";
			name += BENDCHI2_CUTS[bc];
			name += "bendchi2";

			for(int cut = 0; cut < N_PT_CUTS; cut++)
			{
				h[name]->SetBinContent(cut + 1,
					(float) n[name][cut] / nevt * KHZ);
				h[name]->SetBinError(cut + 1,
					sqrt((float) n[name][cut]) / nevt * KHZ);
			}
		}
	}


	// -------------------------------------------------------------------------
	// Output .root file for histograms
	// -------------------------------------------------------------------------

	// TFile* fout = new TFile(type_dir + "miniout_" + type + ".root", "recreate");


	// -------------------------------------------------------------------------
	// Draw and save plots
	// -------------------------------------------------------------------------

	gSystem->mkdir(type_dir + "TrackPlots");
	TString DIR = type_dir + "TrackPlots/";

	TCanvas c;
	TLegend* l;
	TString var;
	TString name1;
	TString name2;
	TString name3;
	TString name4;
	char ctxt[500];
	char ctxt1[500];
	char ctxt2[500];
	char ctxt3[500];
	char ctxt4[500];

	// ---------------------------------------------------------------

	h["trk_pt"]->Draw();
	h["trk_pt"]->SetMinimum(0);
	c.SaveAs(DIR + type + "_trk_pt.pdf");
	// h_trk_pt->Write();

	h["trk_nstub"]->Draw();
	h["trk_nstub"]->SetMinimum(0);
	c.SaveAs(DIR + type + "_trk_nstub.pdf");
	// h_trk_nstub->Write();

	h["trk_chi2"]->Draw();
	h["trk_chi2"]->SetMinimum(0);
	c.SaveAs(DIR + type + "_trk_chi2.pdf");
	// h_trk_chi2->Write();

	h["trk_chi2dof"]->Draw();
	h["trk_chi2dof"]->SetMinimum(0);
	c.SaveAs(DIR + type + "_trk_chi2dof.pdf");
	// h_trk_chi2dof->Write();

	h["trk_bendchi2"]->Draw();
	h["trk_bendchi2"]->SetMinimum(0);
	c.SaveAs(DIR + type + "_trk_bendchi2.pdf");
	// h_trk_bendchi2->Write();

	// ---------------------------------------------------------------

	h["tp_pt"]->Draw();
	h["tp_pt"]->SetMinimum(0);
	c.SaveAs(DIR + type + "_tp_pt.pdf");
	// h_tp_pt->Write();

	// ---------------------------------------------------------------

	h["nevt"]->SetMarkerStyle(20);
	h["nevt"]->Draw("P");

	sprintf(ctxt, "No cuts");

	l = new TLegend(0.6, 0.65, 0.85, 0.85);
	l->SetFillStyle(0);
    l->SetBorderSize(0);
    l->SetTextSize(0.04);
    l->AddEntry(h["nevt"], ctxt, "pl");
    l->SetTextFont(42);
    l->Draw();

	h["nevt"]->SetAxisRange(0.0, KHZ * 1.05, "Y");
	c.SaveAs(DIR + type + "_noCuts_graph.pdf");

	h["nevt"]->SetAxisRange(0.0, 750, "Y");
	c.SaveAs(DIR + type + "_noCuts_lowgraph.pdf");

	h["nevt"]->SetAxisRange(1.0, KHZ * 1.5, "Y");
	gPad->SetLogy(1);
	c.SaveAs(DIR + type + "_noCuts_loggraph.pdf");
	gPad->SetLogy(0);

	// -----------------------------------------------------

	// All chi2dof cuts
	name1 = "nevt_";
	name1 += CHI2DOF_CUTS[0];
	name1 += "chi2dof";
	sprintf(ctxt1, "#chi^{2} < %.1f", CHI2DOF_CUTS[0]);

	name2 = "nevt_";
	name2 += CHI2DOF_CUTS[1];
	name2 += "chi2dof";
	sprintf(ctxt2, "#chi^{2} < %.1f", CHI2DOF_CUTS[1]);

	name3 = "nevt_";
	name3 += CHI2DOF_CUTS[2];
	name3 += "chi2dof";
	sprintf(ctxt3, "#chi^{2} < %.1f", CHI2DOF_CUTS[2]);

	name4 = "nevt_";
	name4 += CHI2DOF_CUTS[3];
	name4 += "chi2dof";
	sprintf(ctxt4, "#chi^{2} < %.1f", CHI2DOF_CUTS[3]);

	var = "_chi2dof";

	compare4Plot(DIR, type, var,
		h["nevt"], ctxt,
		h[name1], ctxt1,
		h[name2], ctxt2,
		h[name3], ctxt3,
		h[name4], ctxt4);

	// -----------------------------------------------------

	// All bendchi2 cuts
	name1 = "nevt_";
	name1 += BENDCHI2_CUTS[0];
	name1 += "bendchi2";
	sprintf(ctxt1, "#chi^{2}_{bend} < %.1f", BENDCHI2_CUTS[0]);

	name2 = "nevt_";
	name2 += BENDCHI2_CUTS[1];
	name2 += "bendchi2";
	sprintf(ctxt2, "#chi^{2}_{bend} < %.1f", BENDCHI2_CUTS[1]);

	name3 = "nevt_";
	name3 += BENDCHI2_CUTS[2];
	name3 += "bendchi2";
	sprintf(ctxt3, "#chi^{2}_{bend} < %.1f", BENDCHI2_CUTS[2]);

	name4 = "nevt_";
	name4 += BENDCHI2_CUTS[3];
	name4 += "bendchi2";
	sprintf(ctxt4, "#chi^{2}_{bend} < %.1f", BENDCHI2_CUTS[3]);

	var = "_bendchi2";

	compare4Plot(DIR, type, var,
		h["nevt"], ctxt,
		h[name1], ctxt1,
		h[name2], ctxt2,
		h[name3], ctxt3,
		h[name4], ctxt4);

	// -----------------------------------------------------

	// All chi2dof constant cuts and bendchi2 cuts
	for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
	{
		name1 = "nevt_";
		name1 += CHI2DOF_CUTS[cd];
		name1 += "chi2dof_";
		name1 += BENDCHI2_CUTS[0];
		name1 += "bendchi2";
		sprintf(ctxt1, "#chi^{2} < %.1f and #chi^{2}_{bend} < %.1f",
			CHI2DOF_CUTS[cd], BENDCHI2_CUTS[0]);

		name2 = "nevt_";
		name2 += CHI2DOF_CUTS[cd];
		name2 += "chi2dof_";
		name2 += BENDCHI2_CUTS[1];
		name2 += "bendchi2";
		sprintf(ctxt2, "#chi^{2} < %.1f and #chi^{2}_{bend} < %.1f",
			CHI2DOF_CUTS[cd], BENDCHI2_CUTS[1]);

		name3 = "nevt_";
		name3 += CHI2DOF_CUTS[cd];
		name3 += "chi2dof_";
		name3 += BENDCHI2_CUTS[2];
		name3 += "bendchi2";
		sprintf(ctxt3, "#chi^{2} < %.1f and #chi^{2}_{bend} < %.1f",
			CHI2DOF_CUTS[cd], BENDCHI2_CUTS[2]);

		name4 = "nevt_";
		name4 += CHI2DOF_CUTS[cd];
		name4 += "chi2dof_";
		name4 += BENDCHI2_CUTS[3];
		name4 += "bendchi2";
		sprintf(ctxt4, "#chi^{2} < %.1f and #chi^{2}_{bend} < %.1f",
			CHI2DOF_CUTS[cd], BENDCHI2_CUTS[3]);

		var = "_";
		var += CHI2DOF_CUTS[cd];
		var += "chi2dof_bendchi2";

		compare4Plot(DIR, type, var,
			h["nevt"], ctxt,
			h[name1], ctxt1,
			h[name2], ctxt2,
			h[name3], ctxt3,
			h[name4], ctxt4);
	}

	// -----------------------------------------------------

	// All bendchi2 constant cuts and chi2dof cuts
	for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
	{
		name1 = "nevt_";
		name1 += CHI2DOF_CUTS[0];
		name1 += "chi2dof_";
		name1 += BENDCHI2_CUTS[bc];
		name1 += "bendchi2";
		sprintf(ctxt1, "#chi^{2}_{bend} < %.1f and #chi^{2} < %.1f",
			BENDCHI2_CUTS[bc], CHI2DOF_CUTS[0]);

		name2 = "nevt_";
		name2 += CHI2DOF_CUTS[1];
		name2 += "chi2dof_";
		name2 += BENDCHI2_CUTS[bc];
		name2 += "bendchi2";
		sprintf(ctxt2, "#chi^{2}_{bend} < %.1f and #chi^{2} < %.1f",
			BENDCHI2_CUTS[bc], CHI2DOF_CUTS[1]);

		name3 = "nevt_";
		name3 += CHI2DOF_CUTS[2];
		name3 += "chi2dof_";
		name3 += BENDCHI2_CUTS[bc];
		name3 += "bendchi2";
		sprintf(ctxt3, "#chi^{2}_{bend} < %.1f and #chi^{2} < %.1f",
			BENDCHI2_CUTS[bc], CHI2DOF_CUTS[2]);

		name4 = "nevt_";
		name4 += CHI2DOF_CUTS[3];
		name4 += "chi2dof_";
		name4 += BENDCHI2_CUTS[bc];
		name4 += "bendchi2";
		sprintf(ctxt4, "#chi^{2}_{bend} < %.1f and #chi^{2} < %.1f",
			BENDCHI2_CUTS[bc], CHI2DOF_CUTS[3]);

		var = "_";
		var += BENDCHI2_CUTS[bc];
		var += "bendchi2_chi2dof";

		compare4Plot(DIR, type, var,
			h["nevt"], ctxt,
			h[name1], ctxt1,
			h[name2], ctxt2,
			h[name3], ctxt3,
			h[name4], ctxt4);
	}

	// ---------------------------------------------------------------

	// Close output .root file
	// fout->Close();

	// Print event rates
	if(print)
	{
		for(int cut = 0; cut < N_PT_CUTS; cut++)
		{
			float event_rate = (float) n["nevt"][cut] / nevt * KHZ;
			cout << "event rate (>=1 track with pt > " << PT_CUTS[cut]
			<< ") = " << event_rate << " kHz" << endl;
		}

		for(int cd = 0; cd < N_CHI2DOF_CUTS; cd++)
		{
			TString name = "nevt_";
			name += CHI2DOF_CUTS[cd];
			name += "chi2dof";

			for(int cut = 0; cut < N_PT_CUTS; cut++)
			{
				float event_rate = (float) n[name][cut] / nevt * KHZ;
				cout << "event rate (>=1 track with pt > " << PT_CUTS[cut]
				<< " && chi2dof < " << CHI2DOF_CUTS[cd]
				<< ") = " << event_rate << " kHz" << endl;
			}
		}

		for(int bc = 0; bc < N_BENDCHI2_CUTS; bc++)
		{
			TString name = "nevt_";
			name += BENDCHI2_CUTS[bc];
			name += "bendchi2";

			for(int cut = 0; cut < N_PT_CUTS; cut++)
			{
				float event_rate = (float) n[name][cut] / nevt * KHZ;
				cout << "event rate (>=1 track with pt > " << PT_CUTS[cut]
				<< " && bendchi2 < " << BENDCHI2_CUTS[bc]
				<< ") = " << event_rate << " kHz" << endl;
			}
		}
	}
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


void compare4Plot(TString dir, TString type, TString var,
	TH1F* h, TString hlabel,
	TH1F* h1, TString h1label,
	TH1F* h2, TString h2label,
	TH1F* h3, TString h3label,
	TH1F* h4, TString h4label)
{
	TCanvas c;
	TLegend* l;

	h->SetMarkerStyle(20);
	h->Draw("P");

	h1->SetMarkerStyle(20);
	h1->SetMarkerColor(2);
	h1->Draw("P SAME");

	h2->SetMarkerStyle(20);
	h2->SetMarkerColor(4);
	h2->Draw("P SAME");

	h3->SetMarkerStyle(20);
	h3->SetMarkerColor(6);
	h3->Draw("P SAME");

	h4->SetMarkerStyle(20);
	h4->SetMarkerColor(8);
	h4->Draw("P SAME");

	l = new TLegend(0.6, 0.65, 0.85, 0.95);
	l->SetFillStyle(0);
    l->SetBorderSize(0);
    l->SetTextSize(0.04);
    l->AddEntry(h, hlabel, "pl");
	l->AddEntry(h1, h1label, "pl");
	l->AddEntry(h2, h2label, "pl");
	l->AddEntry(h3, h3label, "pl");
	l->AddEntry(h4, h4label, "pl");
    l->SetTextFont(42);
    l->Draw();

	h->SetAxisRange(0.0, KHZ * 1.05, "Y");
	h1->SetAxisRange(0.0, KHZ * 1.05, "Y");
	h2->SetAxisRange(0.0, KHZ * 1.05, "Y");
	h3->SetAxisRange(0.0, KHZ * 1.05, "Y");
	h4->SetAxisRange(0.0, KHZ * 1.05, "Y");
	c.SaveAs(dir + type + var + "_graph.pdf");

	h->SetAxisRange(0.0, 750, "Y");
	h1->SetAxisRange(0.0, 750, "Y");
	h2->SetAxisRange(0.0, 750, "Y");
	h3->SetAxisRange(0.0, 750, "Y");
	h4->SetAxisRange(0.0, 750, "Y");
	c.SaveAs(dir + type + var + "_lowgraph.pdf");

	int LOG_MIN = 1.0;
	h->SetAxisRange(LOG_MIN, KHZ * 1.5, "Y");
	h1->SetAxisRange(LOG_MIN, KHZ * 1.5, "Y");
	h2->SetAxisRange(LOG_MIN, KHZ * 1.5, "Y");
	h3->SetAxisRange(LOG_MIN, KHZ * 1.5, "Y");
	h4->SetAxisRange(LOG_MIN, KHZ * 1.5, "Y");
	gPad->SetLogy(1);
	c.SaveAs(dir + type + var + "_loggraph.pdf");
	gPad->SetLogy(0);
}
