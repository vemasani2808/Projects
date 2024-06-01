#include "stdafx.h"
#include "cascinf.h"
#include "snap/kronecker.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>


/*void LoadScoresFromCsv(const TStr& FileName, TFltH& TrustScores, TFltH& EngagementScores, const int& MaxNodes) {
    TSIn SIn(FileName);
    TStr Line;

    TrustScores.Clr();        // Clear existing contents of TrustScores
    EngagementScores.Clr();   // Clear existing contents of EngagementScores

    int NumNodes = 0;

    while (!SIn.Eof() && NumNodes < MaxNodes) {
        SIn.GetNextLn(Line);
        if (Line == "") { break; }

        TStrV FieldsV;
        Line.SplitOnAllCh(',', FieldsV);

        if (FieldsV.Len() >= 3) {
            int NodeId = FieldsV[0].GetInt();
            double Trust = FieldsV[1].GetFlt();
            double Engagement = FieldsV[2].GetFlt();

            // Store values in hash tables
            InfectedBy.AddDat(NodeId, 10);  // Example with InfectedBy hash table

            TrustScores.AddDat(NodeId, Trust);
            EngagementScores.AddDat(NodeId, Engagement);

            NumNodes++;
        }
    }

    printf("All scores read!\n");
}*/



void TNIBs::LoadCascadesTxt(TSIn& SIn) {
	TStr Line;
	while (!SIn.Eof()) {
               SIn.GetNextLn(Line); 
               if (Line=="") { break; }
                TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
	    AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0)); }
	printf("All nodes read!\n");
	while (!SIn.Eof()) { SIn.GetNextLn(Line); AddCasc(Line, Model); }

	printf("All cascades read!\n");
}

void TNIBs::LoadGroundTruthTxt(TSIn& SIn) {
	bool verbose = false;
	TStr Line;

	Network.Clr(); // clear network (if any)

	// add nodes
	while (!SIn.Eof()) {
	        SIn.GetNextLn(Line);
		if (Line=="") { break; }
		TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
		Network.AddNode(NIdV[0].GetInt(), NIdV[1]);
		if (!IsNodeNm(NIdV[0].GetInt())) {
			AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0));
			DomainsIdH.AddDat(NIdV[1]) = NIdV[0].GetInt();
		}
	}

	// add edges
	while (!SIn.Eof()) {
		SIn.GetNextLn(Line);
		TStrV FieldsV; Line.SplitOnAllCh(',', FieldsV);

		TFltFltH Alphas;
		if (FieldsV.Len() == 3) { Alphas.AddDat(0.0) = FieldsV[2].GetFlt(); }
		else {
			for (int i=2; i<FieldsV.Len()-1; i+=2) {
				Alphas.AddDat(FieldsV[i].GetFlt()) = FieldsV[i+1].GetFlt();
			}
		}

		Network.AddEdge(FieldsV[0].GetInt(), FieldsV[1].GetInt(), Alphas);

		if (verbose) {
			printf("Edge %d -> %d: ", FieldsV[0].GetInt(), FieldsV[1].GetInt());
			TFltFltH &AlphasE = Network.GetEDat(FieldsV[0].GetInt(), FieldsV[1].GetInt());
			for (int i=0; i<AlphasE.Len(); i+=2) { printf("(%f, %f)", AlphasE.GetKey(i).Val, AlphasE[i].Val); }
			printf("\n");
		}
	}

	printf("groundtruth nodes:%d edges:%d\n", Network.GetNodes(), Network.GetEdges());
}

void TNIBs::LoadGroundTruthNodesTxt(TSIn& SIn) {
	bool verbose = false;
	TStr Line;

	Network.Clr(); // clear network (if any)

	// add nodes
	while (!SIn.Eof() && Line != "") {
	        SIn.GetNextLn(Line);	
		if (Line=="") { break; }
		TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
		Network.AddNode(NIdV[0].GetInt(), NIdV[1]);
		if (!IsNodeNm(NIdV[0].GetInt())) {
			AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0));
			DomainsIdH.AddDat(NIdV[1]) = NIdV[0].GetInt();
		}
	}

	printf("groundtruth nodes:%d\n", Network.GetNodes());
}

void TNIBs::LoadInferredTxt(TSIn& SIn) {
	bool verbose = false;
	TStr Line;

	InferredNetwork.Clr(); // clear network (if any)

	// add nodes
	while (!SIn.Eof() && Line != "") {
		SIn.GetNextLn(Line);
		if (Line=="") { break; }
		TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
		if (DomainsIdH.IsKey(NIdV[1])) { IAssert(NIdV[0].GetInt()==DomainsIdH.GetDat(NIdV[1]));}
		InferredNetwork.AddNode(NIdV[0].GetInt(), NIdV[1]);
		if (!IsNodeNm(NIdV[0].GetInt())) { AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0)); }
		if (!DomainsIdH.IsKey(NIdV[1])) { DomainsIdH.AddDat(NIdV[1]) = NIdV[0].GetInt(); }
		if (verbose) { printf("Node:%s\n", NIdV[1].CStr()); }
	}

	// add edges
	while (!SIn.Eof()) {
		SIn.GetNextLn(Line);
		TStrV FieldsV; Line.SplitOnAllCh(',', FieldsV);

		TFltFltH Alphas;
		if (FieldsV.Len() == 3) { Alphas.AddDat(0.0) = FieldsV[2].GetFlt(); }
		else {
			for (int i=2; i<FieldsV.Len()-1; i+=2) {
				Alphas.AddDat(FieldsV[i].GetFlt()) = FieldsV[i+1].GetFlt();
			}
		}

		InferredNetwork.AddEdge(FieldsV[0].GetInt(), FieldsV[1].GetInt(), Alphas);

		if (verbose) {
			printf("Edge %d -> %d: ", FieldsV[0].GetInt(), FieldsV[1].GetInt());
			TFltFltH &AlphasE = InferredNetwork.GetEDat(FieldsV[0].GetInt(), FieldsV[1].GetInt());
			for (int i=0; i<AlphasE.Len(); i+=2) { printf("(%f, %f)", AlphasE.GetKey(i).Val, AlphasE[i].Val); }
			printf("\n");
		}
	}

	printf("inferred nodes:%d edges:%d\n", InferredNetwork.GetNodes(), InferredNetwork.GetEdges());
}

void TNIBs::LoadInferredNodesTxt(TSIn& SIn) {
	bool verbose = false;
	TStr Line;

	InferredNetwork.Clr(); // clear network (if any)

	// add nodes
	while (!SIn.Eof()) {
	        SIn.GetNextLn(Line);
		if (Line=="") { break; }
		TStrV NIdV; Line.SplitOnAllCh(',', NIdV);
		if (DomainsIdH.IsKey(NIdV[1])) { IAssert(NIdV[0].GetInt()==DomainsIdH.GetDat(NIdV[1]));}
		InferredNetwork.AddNode(NIdV[0].GetInt(), NIdV[1]);
		if (!IsNodeNm(NIdV[0].GetInt())) { AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0)); }
		if (!DomainsIdH.IsKey(NIdV[1])) { DomainsIdH.AddDat(NIdV[1]) = NIdV[0].GetInt(); }
		SIn.GetNextLn(Line);
	}

	printf("Nodes:%d\n", InferredNetwork.GetNodes());
}

void TNIBs::GenerateGroundTruth(const int& TNetwork, const int& NNodes, const int& NEdges, const TStr& NetworkParams) {
	  bool verbose = true;
	  PNGraph Graph;
	  TKronMtx SeedMtx;
	  TStr MtxNm;

	  // set random seed
	  TInt::Rnd.Randomize();

	  switch (TNetwork) {
	  // 2-dimension kronecker network
	  case 0:
		  printf("Kronecker graph for Ground Truth\n");
		  SeedMtx = TKronMtx::GetMtx(NetworkParams.CStr()); // 0.5,0.5,0.5,0.5

		  printf("\n*** Seed matrix:\n");
		  SeedMtx.Dump();

		  Graph = TKronMtx::GenFastKronecker(SeedMtx, (int)TMath::Log2(NNodes), NEdges, true, 0);

		  break;

	  // forest fire network
	  case 1:
		  printf("Forest Fire graph for Ground Truth\n");
		  TStrV NetworkParamsV; NetworkParams.SplitOnAllCh(';', NetworkParamsV);

		  TFfGGen FF(true, // BurnExpFireP
					 NetworkParamsV[0].GetInt(), // StartNNodes (1)
					 NetworkParamsV[1].GetFlt(), // ForwBurnProb (0.2)
					 NetworkParamsV[2].GetFlt(), // BackBurnProb (0.17)
					 NetworkParamsV[3].GetInt(), // DecayProb (1)
					 NetworkParamsV[4].GetInt(), // Take2AmbasPrb (0)
					 NetworkParamsV[5].GetInt()); // OrphanPrb (0)

		  FF.GenGraph(NNodes, false);
		  Graph = FF.GetGraph();

		  break;
	  }

	  // fill network structure with graph
	  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) { Network.AddNode(NI.GetId()); AddNodeNm(NI.GetId(), TNodeInfo(TStr::Fmt("%d", NI.GetId()), 0)); }
	  for (TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) { if (EI.GetSrcNId()==EI.GetDstNId()) { continue; } Network.AddEdge(EI.GetSrcNId(),EI.GetDstNId(),TFltFltH()); }

	  if (verbose) { printf("Network structure has been generated succesfully!\n"); }
}

void TNIBs::GenerateAlphas(const TStr& RAlphas, const TStr& VaryingType, const double& ResolutionPerEdge, const TStr& VaryingTypeParameter) {
	bool verbose = true;
	int NumPoints = (int)(TotalTime/ResolutionPerEdge + 1.0);
	TStrV RAlphasV; RAlphas.SplitOnAllCh(';', RAlphasV);
	TStrV VaryingTypeStrV; VaryingType.SplitOnAllCh(';', VaryingTypeStrV);
	TStrV VaryingTypeParameterStrV; VaryingTypeParameter.SplitOnAllCh(';', VaryingTypeParameterStrV);

	if (Network.GetNodes() == 0) { printf("Ground truth must be generated before running GenerateAlphas!\n"); return; }

	IAssert(RAlphasV[1].GetFlt()>=RAlphasV[0].GetFlt());

	// set random seed
	TInt::Rnd.Randomize();
	
	for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
		double Alpha = RAlphasV[0].GetFlt() + TFlt::Rnd.GetUniDev() * RAlphasV[1].GetFlt();
		int PeakPoint = TInt::GetMx(1, TInt::Rnd.GetUniDevInt(NumPoints));
		TFltFltH& Alphas = EI();

		// choose trend randomly according to trend distribution
		int trend = 0; double val = TFlt::Rnd.GetUniDev();
		while (val >= 0) { val -= VaryingTypeStrV[trend++].GetFlt(); }
		if (trend!=0) { trend--; }

		int period = (int)(VaryingTypeParameterStrV[0].GetFlt()/ResolutionPerEdge);
		double decay = VaryingTypeParameterStrV[1].GetFlt();
		int phase = TInt::Rnd.GetUniDevInt(period);

		// generate vector of tx rates for the edge
		for (int i=0; i<NumPoints; i++) {
			double CurrentTime = (double)i*ResolutionPerEdge;

			switch ((TVarying)trend) {
				case CONSTANT: // constant tx rate
					Alphas.AddDat(CurrentTime) = Alpha;
					break;
				case LINEAR: // linear trend with maximum at PeakPoint
					if (i<PeakPoint) { Alphas.AddDat(CurrentTime) = Alpha*(double)i/(double)PeakPoint; }
					else { Alphas.AddDat(CurrentTime) = Alpha*(1.0 - (double)(i - PeakPoint)/(double)NumPoints); }
					break;
				case EXPONENTIAL: // exponential trend with maximum at PeakPoint
					if (i<PeakPoint) { Alphas.AddDat(CurrentTime) = Alpha*exp( - decay * (1.0-(double)i/(double)PeakPoint) ); }
					else { Alphas.AddDat(CurrentTime) = Alpha*exp( decay * (1.0-(double)i/(double)PeakPoint) ); }
					break;
				case RAYLEIGH: // rayleigh trend with maximum at PeakPoint
					Alphas.AddDat(CurrentTime) = Alpha*(double)PeakPoint*(double)i/pow((double)PeakPoint,2.0) * exp(-0.5 * pow((double)i/(double)PeakPoint,2.0) );
					break;
				case SLAB: // slab starting at PeakPoint and lasting 5*ResolutionPerEdge
					if (i>=PeakPoint && i<PeakPoint+period/2) { Alphas.AddDat(CurrentTime) = Alpha; }
					else { Alphas.AddDat(CurrentTime) = 0.0; }
					break;
				case SQUARE:
					if ((i+phase)%period < period/2) { Alphas.AddDat(CurrentTime) = 0.0; }
					else { Alphas.AddDat(CurrentTime) = Alpha; }
					break;
				case CHAINSAW:
					if ((i+phase)%period < period/2) { Alphas.AddDat(CurrentTime) = Alpha*(double)((i+phase)%(period/2))/(double)(period/2); }
					else { Alphas.AddDat(CurrentTime) = Alpha*(1.0 - (double)((i+phase)%period - period/2)/(double)(period/2)); }
					break;
				case RANDOM: // random noise (10% over value)
					Alphas.AddDat(CurrentTime) = Alpha + TFlt::Rnd.GetUniDev()*0.1*Alpha;
					break;
				default:
					break;
			}
		}
	}

	if (verbose) { printf("Tx rates has been generated succesfully!\n"); }
}

void TNIBs::AddCasc(const TStr& CascStr, const TModel& Model) {
	int CId = CascH.Len();

	// support cascade id if any
	TStrV FieldsV; CascStr.SplitOnAllCh(';', FieldsV);
	if (FieldsV.Len()==2) { CId = FieldsV[0].GetInt(); }

	// read nodes
    TStrV NIdV; FieldsV[FieldsV.Len()-1].SplitOnAllCh(',', NIdV);
    TCascade C(CId, Model);
    for (int i = 0; i < NIdV.Len(); i+=2) {
      int NId;
      double Tm; 
      NId = NIdV[i].GetInt();
      Tm = NIdV[i+1].GetFlt();
      GetNodeInfo(NId).Vol = GetNodeInfo(NId).Vol + 1;
      C.Add(NId, Tm);
    }
    C.Sort();

    AddCasc(C);
}

void TNIBs::AddCasc(const TIntFltH& Cascade, const int& CId, const TModel& Model) {
	TCascade C(CId, Model);
	for (TIntFltH::TIter NI = Cascade.BegI(); NI < Cascade.EndI(); NI++) {
		GetNodeInfo(NI.GetKey()).Vol = GetNodeInfo(NI.GetKey()).Vol + 1;
		C.Add(NI.GetKey(), NI.GetDat());
	}
	C.Sort();
	AddCasc(C);
}

void TNIBs::GenCascade(TCascade& C) {
	bool verbose = false;
	TIntFltH InfectedNIdH; TIntH InfectedBy;
	double GlobalTime, InitTime;
	double alpha;
	int StartNId;

	if (Network.GetNodes() == 0)
		return;

        // set random seed
        TInt::Rnd.Randomize();

	while (C.Len() < 2) {
		C.Clr();
		InfectedNIdH.Clr();
		InfectedBy.Clr();

		InitTime = TFlt::Rnd.GetUniDev() * TotalTime; // random starting point <TotalTime
		GlobalTime = InitTime;

		StartNId = Network.GetRndNId();
		InfectedNIdH.AddDat(StartNId) = GlobalTime;

		while (true) {
			// sort by time & get the oldest node that did not run infection
			InfectedNIdH.SortByDat(true);
			const int& NId = InfectedNIdH.BegI().GetKey();
			GlobalTime = InfectedNIdH.BegI().GetDat();

			// all the nodes has run infection
			if ( GlobalTime >= TFlt::GetMn(TotalTime, InitTime+Window) )
				break;

			// add current oldest node to the network and set its time
			C.Add(NId, GlobalTime);

			if (verbose) { printf("GlobalTime:%f, infected node:%d\n", GlobalTime, NId); }

			// run infection from the current oldest node
			TStrFltFltHNEDNet::TNodeI NI = Network.GetNI(NId);
			for (int e = 0; e < NI.GetOutDeg(); e++) {
				const int DstNId = NI.GetOutNId(e);

				// choose the current tx rate (we assume the most recent tx rate)
				TFltFltH& Alphas = Network.GetEDat(NId, DstNId);
				for (int j=0; j<Alphas.Len() && Alphas.GetKey(j)<GlobalTime; j++) { alpha = Alphas[j]; }
				if (verbose) { printf("GlobalTime:%f, nodes:%d->%d, alpha:%f\n", GlobalTime, NId, DstNId, alpha); }

				if (alpha<1e-9) { continue; }

				// not infecting the parent
				if (InfectedBy.IsKey(NId) && InfectedBy.GetDat(NId).Val == DstNId)
					continue;

				double sigmaT;
				switch (Model) {
				case EXP:
					// exponential with alpha parameter
					sigmaT = TInt::Rnd.GetExpDev(alpha);
					break;
				case POW:
					// power-law with alpha parameter
					sigmaT = TInt::Rnd.GetPowerDev(1+alpha);
					while (sigmaT < Delta) { sigmaT = Delta*TInt::Rnd.GetPowerDev(1+alpha); }
					break;
				case RAY:
					// rayleigh with alpha parameter
					sigmaT = TInt::Rnd.GetRayleigh(1/sqrt(alpha));
					break;
				default:
					sigmaT = 1;
					break;
				}

				IAssert(sigmaT >= 0);

				double t1 = TFlt::GetMn(GlobalTime + sigmaT, TFlt::GetMn(InitTime+Window, TotalTime));

				if (InfectedNIdH.IsKey(DstNId)) {
					double t2 = InfectedNIdH.GetDat(DstNId);
					if ( t2 > t1 && t2 < TFlt::GetMn(InitTime+Window, TotalTime)) {
						InfectedNIdH.GetDat(DstNId) = t1;
						InfectedBy.GetDat(DstNId) = NId;
					}
				} else {
					InfectedNIdH.AddDat(DstNId) = t1;
					InfectedBy.AddDat(DstNId) = NId;
				}
			}

			// we cannot delete key (otherwise, we cannot sort), so we assign a big time (InitTime + window cut-off)
			InfectedNIdH.GetDat(NId) = TFlt::GetMn(InitTime+Window, TotalTime);
		}
    }

	C.Sort();
}

void TNIBs::GetGroundTruthGraphAtT(const double& Step, PNGraph &GraphAtT) {
	GraphAtT = TNGraph::New();

	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) { GraphAtT->AddNode(NI.GetKey()); }

	for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
		if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }
		double Alpha = 0.0;
		if (EI().IsKey(Step)) { Alpha = EI().GetDat(Step); }

		if (Alpha > MinAlpha) {
			GraphAtT->AddEdge(EI.GetSrcNId(), EI.GetDstNId());
		}
	}
}

void TNIBs::GetGroundTruthNetworkAtT(const double& Step, PStrFltNEDNet& NetworkAtT) {
	NetworkAtT = TStrFltNEDNet::New();

	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) { NetworkAtT->AddNode(NI.GetKey(), NI.GetDat().Name); }

	for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
		if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }
		double Alpha = 0.0;
		if (EI().IsKey(Step)) { Alpha = EI().GetDat(Step); }

		if (Alpha > MinAlpha) {
			NetworkAtT->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), Alpha);
		}
	}
}

void TNIBs::GetInferredGraphAtT(const double& Step, PNGraph &GraphAtT) {
	GraphAtT = TNGraph::New();

	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) { GraphAtT->AddNode(NI.GetKey()); }

	for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
		if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

		double inferredAlpha = 0.0;
		if (EI().IsKey(Step)) { inferredAlpha = EI().GetDat(Step); }

		if (inferredAlpha > MinAlpha) {
			GraphAtT->AddEdge(EI.GetSrcNId(), EI.GetDstNId());
		}
	}
}

void TNIBs::GetInferredNetworkAtT(const double& Step, PStrFltNEDNet& NetworkAtT) {
	NetworkAtT = TStrFltNEDNet::New();

	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
		NetworkAtT->AddNode(NI.GetKey(), NI.GetDat().Name);
	}

	for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
		if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

		double inferredAlpha = 0.0;
		if (EI().IsKey(Step)) { inferredAlpha = EI().GetDat(Step); }

		if (inferredAlpha > MinAlpha) {
			NetworkAtT->AddEdge(EI.GetSrcNId(), EI.GetDstNId(), inferredAlpha);
		}
	}
}

void TNIBs::Init(const TFltV& Steps) {
	// inferred network
	InferredNetwork.Clr();

	// copy nodes from NodeNmH (it will be filled by loading cascades or loading groundtruth)
	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
		InferredNetwork.AddNode(NI.GetKey(), NI.GetDat().Name);
	}

	// performance measures
	PrecisionRecall.Clr();
	Accuracy.Clr();
	MAE.Clr();
	TotalCascadesAlpha.Clr();

	for (int i=0; i<Steps.Len()-1; i++) {
		MAE.Add(TFltPr(Steps[i], 0.0));
		MSE.Add(TFltPr(Steps[i], 0.0));
		Accuracy.Add(TFltPr(Steps[i], 0.0));
		PrecisionRecall.Add(TFltPr(0.0,0.0));
	}
}

void TNIBs::Reset() {
    // reset vectors
	for (int i=0; i<DiffAlphas.Len(); i++) {
		DiffAlphas[i].Clr();
	}
    DiffAlphas.Clr();
    AveDiffAlphas.Clr();
    SampledCascadesH.Clr();
    TotalCascadesAlpha.Clr();
}

void TNIBs::SG(const int& NId, const int& Iters, const TFltV& Steps, const TSampling& Sampling, const TStr& ParamSampling, const bool& PlotPerformance) {
	bool verbose = false;
	int currentCascade = -1;
	TIntIntH SampledCascades;
	TStrV ParamSamplingV; ParamSampling.SplitOnAllCh(';', ParamSamplingV);

	Reset();

	printf("Node %d\n", NId);

	// traverse through all times
	for (int t=1; t<Steps.Len(); t++) {
		// find cascades whose two first infections are earlier than Steps[t]
		TIntFltH CascadesIdx;
		int num_infections = 0;
		for (int i=0; i<CascH.Len(); i++) {
			if (CascH[i].LenBeforeT(Steps[t]) > 1 &&
				( (Sampling!=WIN_SAMPLING && Sampling!=WIN_EXP_SAMPLING) ||
				  (Sampling==WIN_SAMPLING && (Steps[t]-CascH[i].GetMinTm()) <= ParamSamplingV[0].GetFlt()) ||
				  (Sampling==WIN_EXP_SAMPLING && (Steps[t]-CascH[i].GetMinTm()) <= ParamSamplingV[0].GetFlt()) )) {
				num_infections += CascH[i].LenBeforeT(Steps[t]);
				CascadesIdx.AddDat(i) = CascH[i].GetMinTm();
			}
		}

		// if there are not recorded cascades by Steps[t], continue
		if (CascadesIdx.Len()==0) {
			printf("WARNING: No cascades recorded by %f!\n", Steps[t].Val);
			if (PlotPerformance) { ComputePerformanceNId(NId, t, Steps); }
			continue;
		}

		// later cascades first
		CascadesIdx.SortByDat(false);

		printf("Solving step %f: %d cascades, %d infections\n", Steps[t].Val, CascadesIdx.Len(), num_infections);
		SampledCascades.Clr();

		/*if(Steps[t]!=10.000000 && Steps[t]==20.000000){
			//you have to get the inferred network previous edges at the time step 10.00 and then write it to a csv file, pass it to the linkpred and then 
			//write output to a csv file and then take those add those values to the inferred network edge data*/

			

		/*FILE *csvFile = std::fopen("output.csv", "w");
    if(t!=1) {
    for (int a = 0; a <= 512; a++) {
        for (int b = 0; b < 512; b++) {
            // Check if there is an edge between nodes a and b
            if (InferredNetwork.IsEdge(a, b)) {
                // Write the pair (a, b) to the CSV file
                std::fprintf(csvFile, "%d,%d,%f\n", a, b,InferredNetwork.GetEDat(a,b).GetDat(Steps[t]));
            }
        }
    }
	}

	/*int ret_code = std::system("python linkpred.py");

	FILE *csv = std::fopen("new_link.csv", "r");
	 
	int p, q;
    
    // Read data from the CSV file until the end of file (EOF) is reached
    while (std::fscanf(csv, "%d,%d", &p, &q) == 2) {
        // Process the read data (you can print or use it as needed)
		if (!InferredNetwork.IsEdge(p,q)) { InferredNetwork.AddEdge(p,q); }
    }*/



	



			



		// sampling cascades with no replacement
		for (int i=0; i < Iters; i++) {
			switch (Sampling) {
				case UNIF_SAMPLING:
					currentCascade = TInt::Rnd.GetUniDevInt(CascadesIdx.Len());
					break;

				case WIN_SAMPLING:
					currentCascade = TInt::Rnd.GetUniDevInt(CascadesIdx.Len());
					break;

				case EXP_SAMPLING:
					do {
						currentCascade = (int)TFlt::Rnd.GetExpDev(ParamSamplingV[0].GetFlt());
					} while (currentCascade > CascadesIdx.Len()-1);
					break;

				case WIN_EXP_SAMPLING:
					do {
						currentCascade = (int)TFlt::Rnd.GetExpDev(ParamSamplingV[1].GetFlt());
					} while (currentCascade > CascadesIdx.Len()-1);
					break;

				case RAY_SAMPLING:
					do {
						currentCascade = (int)TFlt::Rnd.GetRayleigh(ParamSamplingV[0].GetFlt());
					} while (currentCascade > CascadesIdx.Len()-1);
					break;
			}

			if (!SampledCascades.IsKey(currentCascade)) { SampledCascades.AddDat(currentCascade) = 0; }
			SampledCascades.GetDat(currentCascade)++;

			if (verbose) { printf("Cascade %d sampled!\n", currentCascade); }

			// sampled cascade
			TCascade &Cascade = CascH[CascadesIdx.GetKey(currentCascade)];

			// update gradient and alpha's
			TIntPrV AlphasToUpdate;
			UpdateDiff(OSG, NId, Cascade, AlphasToUpdate, Steps[t]);

			// update alpha's
			for (int j=0; j<AlphasToUpdate.Len(); j++) {
				if (InferredNetwork.IsEdge(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2) &&
				    InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).IsKey(Steps[t])
					) {
					InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) -=
							(Gamma * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1)
									- (Regularizer==1? Mu*InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) : 0.0));

					// project into alpha >= 0
					if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) < Tol) {
						InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = Tol;
					}

					// project into alpha <= MaxAlpha
					if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) > MaxAlpha) {
						InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = MaxAlpha;
					}
				}
			}
			if (verbose) { printf("%d transmission rates updated!\n", AlphasToUpdate.Len()); }
		}

		printf("%d different cascades have been sampled for step %f!\n", SampledCascades.Len(), Steps[t].Val);

		// For alphas that did not get updated, copy last alpha value * aging factor
		int unchanged = 0;
		for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
			if (EI().IsKey(Steps[t]) || t == 0 || !EI().IsKey(Steps[t-1])) { continue; }

			EI().AddDat(Steps[t]) = Aging*EI().GetDat(Steps[t-1]);
			unchanged++;
		}
		if (verbose) { printf("%d transmission rates that did not changed were 'aged' by %f!\n", unchanged, Aging.Val); }

		// compute performance on-the-fly
		if (PlotPerformance) { ComputePerformanceNId(NId, t, Steps); }
	}
}

void TNIBs::BSG(const int& NId, const int& Iters, const TFltV& Steps, const int& BatchLen, const TSampling& Sampling, const TStr& ParamSampling, const bool& PlotPerformance) {
	bool verbose = false;
	int currentCascade = -1;
	TIntIntH SampledCascades;
	TStrV ParamSamplingV; ParamSampling.SplitOnAllCh(';', ParamSamplingV);

	Reset();

	printf("Node %d (|A|: %d)\n", NId, InferredNetwork.GetNodes());

	// traverse through all times (except 0.0; we use 0.0 to compute mse/mae since the inference is delayed)
	for (int t=1; t<Steps.Len(); t++) {
		// find cascades whose two first infections are earlier than Steps[t]
		TIntFltH CascadesIdx;
		int num_infections = 0;
		for (int i = 0; i < CascH.Len(); i++) {
			if (CascH[i].LenBeforeT(Steps[t]) > 1 &&
			      ( (Sampling!=WIN_SAMPLING && Sampling!=WIN_EXP_SAMPLING) ||
					(Sampling==WIN_SAMPLING && (Steps[t]-CascH[i].GetMinTm()) <= ParamSamplingV[0].GetFlt()) ||
					(Sampling==WIN_EXP_SAMPLING && (Steps[t]-CascH[i].GetMinTm()) <= ParamSamplingV[0].GetFlt()) )) {
				num_infections += CascH[i].LenBeforeT(Steps[t]);
				CascadesIdx.AddDat(i) = CascH[i].GetMinTm();
			}
		}

		// if there are not recorded cascades by Steps[t], continue
		if (CascadesIdx.Len() == 0) {
			printf("WARNING: No cascades recorded by %f!\n", Steps[t].Val);
			if (PlotPerformance) { ComputePerformanceNId(NId, t, Steps); }
			continue;
		}

		printf("Solving step %f (%d cascades, %d infections)\n", Steps[t].Val,
				CascadesIdx.Len(), num_infections);

		// sort cascade from most recent to least recent
		CascadesIdx.SortByDat(false);

		// sampling cascades with no replacement
		for (int i=0; i < Iters; i++) {
			// alphas to update
			TIntPrV AlphasToUpdate;

			// delete average gradients
			AveDiffAlphas.Clr();

			// use all cascades for the average gradient
			for (int c=0; c<BatchLen; c++) {
				switch (Sampling) {
					case UNIF_SAMPLING:
						currentCascade = TInt::Rnd.GetUniDevInt(CascadesIdx.Len());
						break;

					case WIN_SAMPLING:
						currentCascade = TInt::Rnd.GetUniDevInt(CascadesIdx.Len());
						break;

					case EXP_SAMPLING:
						do {
							currentCascade = (int)TFlt::Rnd.GetExpDev(ParamSamplingV[0].GetFlt());
						} while (currentCascade > CascadesIdx.Len()-1);
						break;

					case WIN_EXP_SAMPLING:
						do {
							currentCascade = (int)TFlt::Rnd.GetExpDev(ParamSamplingV[1].GetFlt());
						} while (currentCascade > CascadesIdx.Len()-1);
						break;

					case RAY_SAMPLING:
						do {
							currentCascade = (int)TFlt::Rnd.GetRayleigh(ParamSamplingV[0].GetFlt());
						} while (currentCascade > CascadesIdx.Len()-1);
						break;
				}

				// sampled cascade
				TCascade &Cascade = CascH[CascadesIdx.GetKey(currentCascade)];

				if (!SampledCascades.IsKey(currentCascade)) { SampledCascades.AddDat(currentCascade) = 0; }
				SampledCascades.GetDat(currentCascade)++;

				// update gradient and alpha's
				UpdateDiff(OBSG, NId, Cascade, AlphasToUpdate, Steps[t]);
			}

			// update alpha's
			for (int j=0; j<AlphasToUpdate.Len(); j++) {
				if (InferredNetwork.IsEdge(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2) &&
					    InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).IsKey(Steps[t])) {
					switch (Regularizer) {
						case 0:
							InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) -=
								Gamma * (1.0/(double)BatchLen) * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1);
						case 1:
							InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) =
								InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t])*(1.0-Mu*Gamma/(double)BatchLen)
								- Gamma * (1.0/(double)BatchLen) * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1);
					}

					// project into alpha >= 0
					if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) < Tol) {
						InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = Tol;
					}

					// project into alpha <= MaxAlpha
					if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) > MaxAlpha) {
						InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = MaxAlpha;
					}
				}
			}

			// for alphas that did not get updated, copy last alpha value
			int unchanged = 0;
			for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
				if (EI().IsKey(Steps[t]) || t == 0 || !EI().IsKey(Steps[t-1])) { continue; }

				EI().AddDat(Steps[t]) = EI().GetDat(Steps[t-1]);
				unchanged++;
			}
			if (verbose) { printf("%d unchanged transmission rates updated!\n", unchanged); }
		}

		printf("%d different cascades have been sampled for step %f!\n", SampledCascades.Len(), Steps[t].Val);

		// compute performance on-the-fly for each time step
		if (PlotPerformance) { ComputePerformanceNId(NId, t, Steps); }
	}
}

void TNIBs::FG(const int& NId, const int& Iters, const TFltV& Steps) {
	bool verbose = false;
	int currentCascade = -1;
	struct timeval start, end;
	long seconds, useconds;
	double mtime;

	Reset();

	printf("Node %d (|A|: %d)\n", NId, InferredNetwork.GetNodes());

	// traverse through all times
	for (int t=1; t<Steps.Len(); t++) {
		// find cascades whose two first infections are earlier than Steps[t]
		TIntFltH CascadesIdx;
		int num_infections = 0;
		for (int i=0; i<CascH.Len(); i++) {
			if (CascH[i].LenBeforeT(Steps[t]) > 1) {
				num_infections += CascH[i].LenBeforeT(Steps[t]);
				CascadesIdx.AddDat(i) = CascH[i].GetMinTm();
			}
		}

		// if there are not recorded cascades by Steps[t], continue
		if (CascadesIdx.Len()==0) {
			printf("WARNING: No cascades recorded by %f!\n", Steps[t].Val);
//			ComputePerformance(NId, Tol, t, Steps);
			continue;
		}

		printf("Solving step %f (%d cascades, %d infections)\n", Steps[t].Val, CascadesIdx.Len(), num_infections);

		// sort cascade from most recent to least recent
		CascadesIdx.SortByDat(false);

		// sampling cascades with no replacement
		for (int i=0; i < Iters; i++) {
			// alphas to update
			TIntPrV AlphasToUpdate;

			// delete average gradients
			AveDiffAlphas.Clr();

			// use all cascades for the average gradient
			for (int c=0; c<CascadesIdx.Len(); c++) {
				// each cascade
				TCascade &Cascade = CascH[CascadesIdx.GetKey(c)];

				// update gradient and alpha's
				UpdateDiff(OBSG, NId, Cascade, AlphasToUpdate, Steps[t]);
			}

			// update alpha's
			for (int j=0; j<AlphasToUpdate.Len(); j++) {
				if (InferredNetwork.IsEdge(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2) &&
					    InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).IsKey(Steps[t])) {
					switch (Regularizer) {
						case 0:
							InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) -=
								Gamma * (1.0/(double)CascadesIdx.Len()) * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1);
						case 1:
							InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) =
								InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t])*(1.0-Mu*Gamma/(double)CascadesIdx.Len())
								- Gamma * (1.0/(double)CascadesIdx.Len()) * AveDiffAlphas.GetDat(AlphasToUpdate[j].Val1);
					}

					// project into alpha >= 0
					if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) < Tol) {
						InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = Tol;
					}

					// project into alpha <= MaxAlpha
					if (InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) > MaxAlpha) {
						InferredNetwork.GetEDat(AlphasToUpdate[j].Val1, AlphasToUpdate[j].Val2).GetDat(Steps[t]) = MaxAlpha;
					}
				}
			}

			// for alphas that did not get updated, copy last alpha value
			int unchanged = 0;
			for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
				if (EI().IsKey(Steps[t]) || t == 0 || !EI().IsKey(Steps[t-1])) { continue; }

				EI().AddDat(Steps[t]) = EI().GetDat(Steps[t-1]);
				unchanged++;
			}
			if (verbose) { printf("%d unchanged transmission rates updated!\n", unchanged); }
		}

		// compute performance on-the-fly for each time step
		ComputePerformanceNId(NId, t, Steps);
	}
}

void TNIBs::UpdateDiff(const TOptMethod& OptMethod, const int& NId, TCascade& Cascade, TIntPrV& AlphasToUpdate, const double& CurrentTime) {
	IAssert(InferredNetwork.IsNode(NId));
 
	

	//LoadScoresFromCsv("your_file.csv", TrustScores, EngagementScores, 512);

	double sum = 0.0;

	//importing the trust and engagement scores from an array
	float TrustScores[513]={0.1, 2.9, 0.34, 0.1, 0.12, 3.0, 4.88, 0.68, 3.07, 1.3, 3.89, 1.62, 1.52, 3.07, 3.89, 1.67, 3.09, 1.69, 3.14, 1.45, 4.52, 1.38, 0.48, 3.79, 3.29, 2.95, 2.46, 6.64, 5.07, 5.48, 0.39, 0.48, 0.19, 0.14, 1.64, 3.55, 0.89, 1.5, 4.42, 5.29, 1.33, 2.83, 1.35, 8.38, 5.34, 1.06, 0.19, 3.84, 3.79, 1.21, 5.0, 1.23, 0.65, 1.62, 2.63, 0.0, 0.12, 1.86, 0.19, 3.84, 2.85, 2.25, 5.48, 0.19, 1.67, 0.19, 3.57, 3.84, 3.12, 6.43, 0.0, 0.17, 4.15, 7.05, 2.0, 2.63, 1.26, 7.34, 3.29, 4.49, 1.69, 2.08, 3.29, 6.38, 0.39, 3.74, 3.86, 6.01, 0.68, 0.0, 1.96, 0.1, 2.13, 3.19, 4.18, 2.95, 1.38, 0.8, 2.37, 0.68, 0.41, 0.68, 0.41, 0.43, 1.33, 0.34, 5.94, 4.64, 2.39, 1.71, 1.52, 4.03, 4.25, 0.19, 3.74, 7.25, 1.79, 2.9, 0.0, 0.0, 0.19, 2.15, 6.79, 4.03, 4.59, 1.98, 3.62, 4.18, 0.99, 0.36, 5.75, 4.13, 5.29, 1.67, 0.8, 0.85, 2.75, 2.15, 5.14, 1.23, 1.26, 2.2, 2.9, 0.29, 5.43, 1.11, 3.53, 3.33, 0.0, 1.3, 1.98, 0.1, 0.34, 0.19, 4.69, 1.35, 5.07, 0.29, 0.46, 0.05, 3.0, 3.89, 0.0, 3.53, 0.29, 1.47, 3.21, 0.39, 4.66, 2.34, 3.36, 6.09, 1.52, 4.03, 0.68, 4.13, 0.68, 0.77, 1.28, 1.18, 0.0, 0.1, 3.26, 3.09, 3.09, 0.29, 2.9, 1.76, 0.07, 0.8, 0.19, 0.19, 2.49, 3.21, 1.04, 4.42, 3.36, 1.69, 0.24, 0.99, 0.0, 4.54, 0.22, 1.35, 0.1, 5.05, 2.49, 3.99, 1.4, 4.32, 3.33, 2.49, 3.99, 1.5, 0.39, 2.37, 0.87, 4.3, 2.37, 0.12, 1.38, 7.56, 3.12, 2.63, 7.25, 1.35, 1.4, 0.1, 5.24, 2.58, 0.99, 0.22, 0.29, 0.14, 2.08, 7.49, 6.47, 1.57, 0.0, 3.04, 0.43, 3.21, 3.84, 0.19, 6.69, 1.5, 2.92, 3.14, 2.32, 0.19, 3.33, 1.18, 4.08, 0.17, 4.3, 0.6, 2.42, 0.07, 4.15, 2.46, 0.0, 0.36, 0.39, 0.19, 0.1, 2.03, 5.92, 5.89, 1.45, 3.77, 0.0, 0.39, 1.57, 3.33, 1.76, 1.84, 1.35, 0.0, 2.22, 5.63, 3.55, 0.19, 2.85, 4.11, 3.26, 0.0, 2.56, 2.42, 0.1, 1.69, 1.06, 4.98, 3.12, 4.03, 2.73, 0.82, 0.19, 5.85, 0.39, 2.73, 3.04, 5.6, 4.83, 0.63, 0.99, 3.77, 1.67, 1.14, 0.39, 0.1, 3.55, 2.56, 0.34, 2.9, 2.37, 1.43, 0.77, 0.0, 1.98, 1.21, 2.61, 6.3, 4.98, 1.81, 0.1, 2.8, 1.93, 0.94, 0.0, 4.32, 2.73, 0.29, 0.05, 2.32, 3.24, 3.57, 2.63, 3.55, 5.19, 3.0, 2.78, 0.1, 1.16, 1.55, 1.98, 0.0, 3.12, 0.29, 0.58, 1.88, 0.0, 0.39, 0.68, 4.88, 2.95, 0.48, 0.8, 2.75, 3.86, 0.19, 3.24, 0.43, 5.0, 3.48, 0.99, 4.57, 5.34, 1.71, 0.14, 0.1, 2.29, 1.01, 1.84, 1.09, 2.66, 5.12, 1.57, 10.0, 0.0, 6.5, 0.1, 0.29, 0.29, 5.7, 2.85, 2.8, 3.16, 2.08, 3.7, 2.39, 3.79, 2.22, 0.12, 5.39, 0.39, 1.93, 0.19, 4.93, 1.55, 2.05, 2.95, 2.29, 2.85, 2.54, 3.0, 0.39, 0.39, 1.23, 0.39, 2.08, 0.46, 1.55, 0.1, 3.41, 2.27, 1.38, 0.19, 1.14, 2.56, 5.0, 4.01, 4.35, 3.6, 3.09, 5.6, 1.93, 2.42, 1.52, 0.39, 1.93, 2.17, 1.88, 2.66, 4.13, 3.96, 3.12, 1.93, 0.1, 1.84, 0.17, 0.39, 2.68, 2.95, 5.0, 1.59, 5.12, 2.32, 3.07, 0.22, 2.15, 1.71, 0.29, 1.33, 0.58, 0.0, 2.61, 2.58, 0.1, 1.98, 5.56, 2.68, 1.52, 3.38, 2.63, 1.74, 1.91, 2.27, 3.07, 1.59, 4.66, 0.1, 3.45, 3.77, 2.51, 1.98, 1.62, 1.84, 2.71, 0.29, 0.94, 0.58, 2.08, 3.6, 6.01, 1.11, 2.71, 2.27, 5.05, 0.0, 5.1, 1.74, 0.0, 2.49, 1.84, 5.8, 3.74, 0.53, 0.1, 1.84, 2.08, 1.59, 2.15, 1.26, 2.63, 2.15, 2.44, 0.0, 4.52, 1.74, 2.56, 3.74, 1.81, 0.0};
    float EngagementScores[513]= {0.01, 6.42, 3.08, 0.01, 4.32, 6.01, 6.56, 4.01, 6.63, 5.32, 6.49, 5.58, 5.44, 7.2, 7.08, 6.14, 5.53, 5.04, 5.05, 4.95, 5.73, 4.62, 7.89, 6.62, 7.18, 4.67, 6.39, 6.55, 6.61, 6.01, 1.38, 4.45, 0.02, 4.28, 5.37, 5.39, 3.46, 4.28, 7.95, 6.84, 5.05, 6.56, 4.41, 7.19, 6.5, 4.4, 0.02, 5.74, 7.32, 5.57, 6.77, 4.59, 3.97, 4.24, 5.94, 0.0, 5.11, 5.24, 0.02, 5.22, 6.35, 4.9, 6.31, 1.29, 5.28, 0.02, 6.09, 5.59, 6.55, 7.03, 0.0, 2.08, 5.99, 7.3, 5.08, 4.7, 4.74, 6.58, 5.39, 6.07, 5.86, 4.52, 5.52, 7.28, 6.59, 6.96, 5.61, 6.4, 3.66, 0.0, 4.2, 0.01, 4.77, 6.44, 5.79, 6.79, 4.54, 3.8, 5.58, 5.4, 3.39, 3.51, 5.48, 3.57, 4.58, 4.14, 6.68, 6.4, 5.3, 4.36, 3.78, 7.2, 5.92, 6.86, 6.0, 7.77, 5.01, 5.15, 0.0, 0.0, 0.02, 5.19, 7.37, 7.3, 6.5, 4.75, 5.15, 6.3, 3.29, 4.56, 7.13, 5.92, 6.64, 4.57, 3.46, 2.94, 4.98, 6.19, 7.03, 4.84, 2.44, 5.13, 6.55, 0.16, 6.95, 4.75, 7.01, 6.6, 0.0, 4.44, 5.6, 0.01, 2.08, 0.02, 5.72, 3.93, 6.33, 8.44, 1.95, 1.51, 6.34, 5.43, 0.0, 5.05, 2.28, 4.39, 6.53, 2.85, 5.91, 4.85, 5.5, 7.06, 4.95, 6.36, 4.03, 6.74, 5.98, 5.09, 3.64, 4.78, 0.0, 0.01, 5.97, 6.47, 6.22, 2.5, 5.25, 4.56, 1.29, 3.1, 0.02, 0.02, 4.87, 5.39, 3.5, 6.12, 6.68, 5.01, 3.99, 3.28, 0.0, 6.17, 0.07, 3.63, 0.01, 6.0, 5.6, 7.82, 4.14, 5.97, 5.38, 6.11, 7.42, 4.61, 0.94, 5.55, 3.99, 6.23, 4.34, 4.66, 4.39, 7.0, 4.85, 6.48, 7.48, 4.85, 3.31, 0.01, 6.32, 5.04, 3.5, 0.93, 1.3, 1.19, 4.04, 7.41, 6.94, 4.3, 0.0, 6.08, 3.83, 6.8, 5.48, 0.02, 6.48, 5.62, 6.11, 5.91, 5.78, 0.02, 6.89, 4.6, 7.4, 4.62, 6.08, 4.94, 6.15, 5.34, 7.72, 5.92, 0.0, 6.12, 10.0, 0.02, 0.01, 5.63, 6.68, 6.69, 4.39, 5.23, 0.0, 5.28, 4.28, 4.82, 5.36, 5.75, 4.87, 0.0, 6.41, 6.93, 5.32, 0.02, 6.22, 5.28, 6.84, 0.0, 5.75, 5.38, 0.01, 5.31, 3.26, 6.91, 5.23, 6.23, 5.1, 4.42, 0.02, 7.45, 6.24, 6.36, 5.55, 6.8, 8.22, 2.73, 5.23, 5.77, 4.75, 3.79, 0.05, 0.01, 7.13, 5.99, 2.44, 4.98, 6.02, 4.46, 4.43, 0.0, 5.63, 3.09, 5.7, 7.26, 6.51, 4.52, 0.11, 4.95, 4.52, 4.68, 0.0, 5.57, 6.47, 8.98, 4.94, 5.62, 5.72, 6.7, 6.06, 6.95, 5.72, 4.52, 6.39, 0.01, 4.44, 4.66, 3.88, 0.0, 6.46, 0.72, 3.59, 4.98, 0.0, 2.49, 5.49, 6.33, 4.76, 3.54, 3.83, 5.04, 5.27, 0.02, 4.91, 3.29, 6.13, 5.68, 4.28, 6.45, 6.97, 4.18, 4.29, 0.01, 6.21, 3.69, 4.49, 4.61, 6.0, 6.35, 5.02, 7.38, 0.0, 7.32, 0.01, 2.35, 1.25, 6.84, 5.07, 5.52, 6.89, 4.99, 6.81, 4.82, 5.66, 5.88, 2.07, 6.47, 1.44, 4.26, 0.02, 6.13, 4.25, 5.48, 5.5, 4.66, 5.34, 5.27, 5.29, 4.22, 3.11, 4.78, 2.71, 4.62, 4.49, 4.95, 0.01, 7.05, 4.3, 5.37, 0.02, 5.38, 6.38, 6.31, 7.36, 5.82, 6.86, 6.73, 7.13, 4.56, 5.18, 4.88, 3.51, 4.35, 4.19, 4.93, 6.4, 5.5, 5.58, 5.48, 5.56, 0.01, 4.63, 1.86, 8.63, 4.72, 5.2, 6.42, 4.61, 6.12, 5.12, 5.61, 2.25, 4.98, 4.38, 2.33, 4.33, 2.46, 0.0, 4.57, 5.54, 0.01, 3.48, 6.05, 6.63, 4.42, 5.9, 5.88, 4.56, 4.82, 5.51, 5.52, 5.39, 6.11, 0.01, 5.18, 4.55, 6.27, 3.8, 4.23, 5.18, 4.72, 1.12, 4.4, 2.21, 5.01, 6.03, 6.64, 3.61, 6.02, 5.58, 6.22, 0.0, 6.04, 4.38, 0.0, 5.42, 4.91, 6.58, 5.09, 4.87, 0.01, 4.31, 4.89, 3.22, 4.96, 3.93, 4.38, 5.29, 6.03, 0.0, 8.3, 5.09, 6.02, 5.31, 4.41, 0.0};
	//printf("%f",TrustScores[0]);

	/*FILE* file = fopen("output_file1.csv", "r");

	char line[1024]; // Adjust the buffer size accordingly

    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        while (token != nullptr) {
            printf("%s\t", token);
            token = strtok(nullptr, ",");
        }
        printf("\n");
    }

    fclose(file);*/

	//int ret_code = std::system("python linkpred.py");

   //if(CurrentTime!=10.00){}
	// we assume cascade is sorted & iterator returns in sorted order
	if (Cascade.IsNode(NId) && Cascade.GetTm(NId) <= CurrentTime) {
		for (THash<TInt, THitInfo>::TIter NI = Cascade.BegI(); NI < Cascade.EndI(); NI++) {
			// consider only nodes that are earlier in time
			if ( (Cascade.GetTm(NId)<=NI.GetDat().Tm) ||
			     (Cascade.GetTm(NId)-Delta<=NI.GetDat().Tm && Model==POW)
			   ) { break; }

			TIntPr Pair(NI.GetKey(), NId);

			// if edge/alpha doesn't exist, create
			if (!InferredNetwork.IsEdge(Pair.Val1, Pair.Val2)) { InferredNetwork.AddEdge(Pair.Val1, Pair.Val2, TFltFltH()); }
			if (!InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).IsKey(CurrentTime)) {
				InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).AddDat(CurrentTime) = InitAlpha;
			}

			switch(Model) {
				case EXP: // exponential
					sum += InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).GetDat(CurrentTime).Val;
					break;
				case POW: // powerlaw
					sum += InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).GetDat(CurrentTime).Val/(Cascade.GetTm(NId)-NI.GetDat().Tm);
					break;
				case RAY: // rayleigh
					sum += InferredNetwork.GetEDat(Pair.Val1, Pair.Val2).GetDat(CurrentTime).Val*(Cascade.GetTm(NId)-NI.GetDat().Tm);
					break;
				default:
					sum = 0.0;
			}
		}
	}

	// we assume cascade is sorted & iterator returns in sorted order
	for (THash<TInt, THitInfo>::TIter NI = Cascade.BegI(); NI < Cascade.EndI(); NI++) {
		// only consider nodes that are earlier in time if node belongs to the cascade
		if ( Cascade.IsNode(NId) && (Cascade.GetTm(NId)<=NI.GetDat().Tm ||
				(Cascade.GetTm(NId)-Delta<=NI.GetDat().Tm && Model==POW))
		   ) { break; }

		// consider infected nodes up to CurrentTime
		if (NI.GetDat().Tm > CurrentTime) { break; }

		TIntPr Pair(NI.GetKey(), NId); // potential edge

		double val = 0.0;

		if (Cascade.IsNode(NId) && Cascade.GetTm(NId) <= CurrentTime) {
			IAssert((Cascade.GetTm(NId) - NI.GetDat().Tm) > 0.0);

			switch(Model) { // compute gradients for infected
				case EXP: // exponential
					val = (Cascade.GetTm(NId) - NI.GetDat().Tm) - 1.0/sum;
					break;
				case POW: // powerlaw
					val = log((Cascade.GetTm(NId) - NI.GetDat().Tm)/Delta) - 1.0/((Cascade.GetTm(NId)-NI.GetDat().Tm)*sum);
					break;
				case RAY: // rayleigh
					val = TMath::Power(Cascade.GetTm(NId)-NI.GetDat().Tm, 2.0)/2.0 - (Cascade.GetTm(NId)-NI.GetDat().Tm)/sum;
					break;
				default:
					val = 0.0;
			}
		} else { // compute gradients for non infected
			IAssert((CurrentTime - NI.GetDat().Tm) >= 0.0);

			switch(Model) {
				case EXP: // exponential
					val = (CurrentTime-NI.GetDat().Tm);
					// if every cascade was recorded up to a maximum Window cut-off
					if ( (Window > -1) && (CurrentTime-Cascade.GetMinTm() > Window) ) { val = (Cascade.GetMinTm()+Window-NI.GetDat().Tm); }
					break;
				case POW: // power-law
					val = TMath::Mx(log((CurrentTime-NI.GetDat().Tm)/Delta), 0.0);
					// if every cascade was recorded up to a maximum Window cut-off
					if ( (Window > -1) && (CurrentTime-Cascade.GetMinTm() > Window) ) { val = TMath::Mx(log((Cascade.GetMinTm()+Window-NI.GetDat().Tm)/Delta), 0.0); }
					break;
				case RAY: // rayleigh
					val = TMath::Power(CurrentTime-NI.GetDat().Tm,2.0)/2.0;
					// if every cascade was recorded up to a maximum Window cut-off
					if ( (Window > -1) && (CurrentTime-Cascade.GetMinTm() > Window) ) { val = TMath::Power(Cascade.GetMinTm()+Window-NI.GetDat().Tm,2.0)/2.0; }
					break;
				default:
					val = 0.0;
			}
		}

		if (!AveDiffAlphas.IsKey(Pair.Val1)) { AveDiffAlphas.AddDat(Pair.Val1) = 0.0; }

		switch (OptMethod) {
			case OBSG:
			case OEBSG:
			case OFG:
				// update stochastic average gradient (based on batch for OBSG and OEBSG and based on all cascades for FG)
				AveDiffAlphas.GetDat(Pair.Val1) += val;
				break;
			case OSG:
			case OESG:
				// update stochastic gradient (we use a single gradient due to current cascade)
				AveDiffAlphas.GetDat(Pair.Val1) = val;
			default:
				break;
		}

		AlphasToUpdate.Add(Pair);
	}

	return;
}

void TNIBs::find_C( int t, TFltV &x, TFltVV &C, const int& k, const double& s, const double& gamma, const double& T ){
	if ( t >= x.Len() ) return;
	if ( t == 0 ){
		C = TFltVV( x.Len(), k );
	}else{
		int n = x.Len() - 1;
		for (int j = 0; j < k; j++){
			double alpha = ( (x.Len() ) / T ) * pow( s, j );
			double term_1 = -log(alpha) + alpha * x[t];
			double term_2 = 0;
			if ( t == 1 ){
				term_2 = j * log(n) * gamma;
			}
			else{
				bool first = false;
				for (int l = 0; l < k; l++){
					double my_val = C(t-1, l);
					if ( j > l ) my_val += (j - l) * log(n) * gamma;
					if ( !first || my_val < term_2 ){
						term_2 = my_val;
						first = true;
					}
				}
			}
			C( t, j ) = term_1 + term_2;
		}
	}
	find_C( t + 1, x, C, k, s, gamma, T );
}

void TNIBs::find_min_state( TFltVV &C, TIntV &states, const int& k, const double& s, const double& gamma, const double& T ){
	states = TIntV( C.GetRows() );
	states[0] = 0;
	int n = C.GetRows() - 1;
	for (int t = C.GetRows() - 1; t > 0; t --){
		double best_val = 0;
		int best_state = -1;
		for (int j = 0; j < C.GetCols(); j++){
			double c_state = C( t, j );
			if ( t < C.GetRows() - 2 && states[t+1] > j ){
				c_state += ( states[t+1] - j ) * gamma * log(n);
			}
			if ( best_state == -1 || best_val > c_state ){
				best_state = j;
				best_val = c_state;
			}
		}
		states[t] = best_state;
	}
}

void TNIBs::LabelBurstAutomaton( const int& SrcId, const int& DstId, TIntV &state_labels, TFltV &state_times, const bool& inferred, const int& k, const double& s, const double& gamma, const TSecTm& MinTime, const TSecTm& MaxTime ){
	TVec<TSecTm> arrival_times;
	TFltFltH &LinksEdge = (inferred? InferredNetwork.GetEDat(SrcId, DstId) : Network.GetEDat(SrcId, DstId));

	for (int i=0; i<LinksEdge.Len(); i++) {
		if (LinksEdge[i]>MinAlpha) {
			TSecTm tsecs;
			tsecs = (uint)LinksEdge.GetKey(i); // link rates is in seconds
			if (tsecs > MaxTime || tsecs < MinTime) { continue; }
			arrival_times.Add(tsecs);
		}
	}

	if ( arrival_times.Len() < 2 ) return;

	TFltV x;
	x.Add( 0 );
	arrival_times.Sort(true);
	double T = ((double)arrival_times.Last().GetAbsSecs()) - ((double)arrival_times[0].GetAbsSecs());
	for (int i = 1; i < arrival_times.Len(); i++){
		x.Add( ((double)arrival_times[i].GetAbsSecs()) - ((double)arrival_times[i-1].GetAbsSecs()) );
	}
	TFltVV Cost_matrix;
	//printf("arrivals = %d\n", x.Len() );
	find_C( 0, x, Cost_matrix, k, s, gamma, T);

	find_min_state( Cost_matrix, state_labels, k, s, gamma, T );

	for (int i=0; i<state_labels.Len(); i++) { state_times.Add((double)arrival_times[i].GetAbsSecs()); }
}


void TNIBs::ComputePerformanceNId(const int& NId, const int& t, const TFltV& Steps) {
	double CurrentMAE = 0.0;
	double CurrentMSE = 0.0;
	TFltPr CurrentPrecisionRecall(0.0, 0.0);
	double CurrentAccD = 0.0;

	TStrFltFltHNEDNet::TNodeI NI = InferredNetwork.GetNI(NId);
	for (int i=0; i<NI.GetInDeg(); i++) {
		double inferredAlpha = InferredNetwork.GetEDat(NI.GetInNId(i), NId).GetDat(Steps[t]);
		// we get the true alpha in the previous step (the inferred network contains delayed estimates)
		double trueAlpha = 0.0;
		if (Network.IsEdge(NI.GetInNId(i), NId) && Network.GetEDat(NI.GetInNId(i), NId).IsKey(Steps[t-1])) { trueAlpha = Network.GetEDat(NI.GetInNId(i), NId).GetDat(Steps[t-1]); }

		// accuracy (denominator)
		CurrentAccD += (double) (inferredAlpha > MinAlpha);

		// precision
		CurrentPrecisionRecall.Val2 += (double) (inferredAlpha > MinAlpha && trueAlpha<MinAlpha);
	}

	NI = Network.GetNI(NId);
	int NumEdges = 0;
	for (int i=0; i<NI.GetInDeg(); i++) {
		TIntPr Pair(NI.GetInNId(i), NId);

		// we get the inferred Alpha if any
		double inferredAlpha = 0.0;
		if (InferredNetwork.IsEdge(NI.GetInNId(i), NId) && InferredNetwork.GetEDat(NI.GetInNId(i), NId).IsKey(Steps[t])) {
			inferredAlpha = InferredNetwork.GetEDat(NI.GetInNId(i), NId).GetDat(Steps[t]).Val;
		}

		// we get the true alpha in the previous step (the inferred network contains delayed estimates)
		double trueAlpha = 0.0;
		if (Network.GetEDat(NI.GetInNId(i), NId).IsKey(Steps[t-1])) { trueAlpha = Network.GetEDat(NI.GetInNId(i), NId).GetDat(Steps[t-1]); }

		// mae
		if (trueAlpha > MinAlpha) {
			NumEdges++;
			CurrentMAE += fabs(trueAlpha - TFlt::GetMn(inferredAlpha, MaxAlpha))/trueAlpha;
		}

		// mse
		CurrentMSE += pow(trueAlpha - TFlt::GetMn(inferredAlpha, MaxAlpha), 2.0);

		// recall
		CurrentPrecisionRecall.Val1 += (double) (inferredAlpha > MinAlpha && trueAlpha > MinAlpha);
	}

	if (NumEdges > 0) {
		MAE[t-1].Val2 += CurrentMAE / ((double)(NumEdges*Network.GetNodes()));
		MSE[t-1].Val2 += CurrentMSE / ((double)(NumEdges*Network.GetNodes()));
		PrecisionRecall[t-1].Val1 += CurrentPrecisionRecall.Val1/(double)(NumEdges*Network.GetNodes());
	}

	if (CurrentAccD > 0) {
		PrecisionRecall[t-1].Val2 += (1.0 - CurrentPrecisionRecall.Val2 / CurrentAccD)/(double)Network.GetNodes();
	} else {
		PrecisionRecall[t-1].Val2 += 1.0/(double)Network.GetNodes();
	}

	Accuracy[t-1].Val2 = 1.0 - (1.0-PrecisionRecall[t-1].Val2)/(PrecisionRecall[t-1].Val2 * (1.0/PrecisionRecall[t-1].Val2 + 1.0/PrecisionRecall[t-1].Val1)) - (1.0-PrecisionRecall[t-1].Val1)/(PrecisionRecall[t-1].Val1* (1.0/PrecisionRecall[t-1].Val2 + 1.0/PrecisionRecall[t-1].Val1));
}

void TNIBs::SaveInferredPajek(const TStr& OutFNm, const double& Step, const TIntV& NIdV) {
    TFOut FOut(OutFNm);
    FOut.PutStr(TStr::Fmt("*Vertices %d\r\n", NodeNmH.Len()));
    for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
    	if (NIdV.Len() > 0 && !NIdV.IsIn(NI.GetKey())) { continue; }

    	FOut.PutStr(TStr::Fmt("%d \"%s\" ic Blue\r\n", NI.GetKey().Val+1, NI.GetDat().Name.CStr()));
    }
    FOut.PutStr("*Arcs\r\n");
    for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
    	if (NIdV.Len() > 0 && (!NIdV.IsIn(EI.GetSrcNId()) || !NIdV.IsIn(EI.GetDstNId()))) { continue; }
    	if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

    	double inferredAlpha = 0.0;
    	if (EI().IsKey(Step)) { inferredAlpha = EI().GetDat(Step); }

    	if (inferredAlpha > MinAlpha) {
    		FOut.PutStr(TStr::Fmt("%d %d %f\r\n", EI.GetSrcNId()+1, EI.GetDstNId()+1, (inferredAlpha > MaxAlpha? MaxAlpha.Val : inferredAlpha)));
    	}
    }
}

void TNIBs::SaveInferred(const TStr& OutFNm, const TIntV& NIdV) {
	TFOut FOut(OutFNm);

	// write nodes to file
	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
		if (NIdV.Len() > 0 && !NIdV.IsIn(NI.GetKey())) { continue; }

		FOut.PutStr(TStr::Fmt("%d,%s\r\n", NI.GetKey().Val, NI.GetDat().Name.CStr()));
	}

	FOut.PutStr("\r\n");

	// write edges to file (not allowing self loops in the network)
	for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
		if (NIdV.Len() > 0 && (!NIdV.IsIn(EI.GetSrcNId()) || !NIdV.IsIn(EI.GetDstNId()))) { continue; }
		if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

		// not allowing self loops in the Kronecker network
		if (EI.GetSrcNId() != EI.GetDstNId()) {
			if (EI().Len() > 0) {
				TStr Line; bool IsEdge = false;
				for (int i=0; i<EI().Len(); i++) {
					if (EI()[i]>MinAlpha) {
						Line += TStr::Fmt(",%f,%f", EI().GetKey(i).Val, (EI()[i] > MaxAlpha? MaxAlpha.Val : EI()[i].Val) );
						IsEdge = true;
					} else { // we write 0 explicitly
						Line += TStr::Fmt(",%f,0.0", EI().GetKey(i).Val);
					}
				}
				// if none of the alphas is bigger than 0, no edge is written
				if (IsEdge) {
					FOut.PutStr(TStr::Fmt("%d,%d", EI.GetSrcNId(), EI.GetDstNId()));
					FOut.PutStr(Line);
					FOut.PutStr("\r\n");
				}
			}
			else
				FOut.PutStr(TStr::Fmt("%d,%d,1\r\n", EI.GetSrcNId(), EI.GetDstNId()));
		}
	}
}

void TNIBs::SaveInferred(const TStr& OutFNm, const double& Step, const TIntV& NIdV) {
	TFOut FOut(OutFNm);

	// write nodes to file
	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
		if (NIdV.Len() > 0 && !NIdV.IsIn(NI.GetKey())) { continue; }

		FOut.PutStr(TStr::Fmt("%d,%s\r\n", NI.GetKey().Val, NI.GetDat().Name.CStr()));
	}
	FOut.PutStr("\r\n");

	// write edges to file (not allowing self loops in the network)
	for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
		if (NIdV.Len() > 0 && (!NIdV.IsIn(EI.GetSrcNId()) || !NIdV.IsIn(EI.GetDstNId()))) { continue; }
		if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

		// not allowing self loops in the Kronecker network
		if (EI.GetSrcNId() != EI.GetDstNId()) {
			double inferredAlpha = 0.0;
			if (EI().IsKey(Step)) { inferredAlpha = EI().GetDat(Step); }

			if (inferredAlpha > MinAlpha) {
				FOut.PutStr(TStr::Fmt("%d,%d,%f\r\n", EI.GetSrcNId(), EI.GetDstNId(), (inferredAlpha > MaxAlpha? MaxAlpha.Val : inferredAlpha)));
			}
		}
	}
}

void TNIBs::SaveInferredEdges(const TStr& OutFNm) {
	TFOut FOut(OutFNm);

	// write edges to file (not allowing self loops in the network)
	for (TStrFltFltHNEDNet::TEdgeI EI = InferredNetwork.BegEI(); EI < InferredNetwork.EndEI(); EI++) {
		if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

		// not allowing self loops in the Kronecker network
		if (EI.GetSrcNId() != EI.GetDstNId()) {
			if (EI().Len() > 0) {
				TStr Line; bool IsEdge = false;
				for (int i=0; i<EI().Len(); i++) {
					if (EI()[i]>MinAlpha) {
						Line += TStr::Fmt(",%f,%f", EI().GetKey(i).Val, (EI()[i] > MaxAlpha? MaxAlpha.Val : EI()[i].Val) );
						IsEdge = true;
					} else { // we write 0 explicitly
						Line += TStr::Fmt(",%f,0.0", EI().GetKey(i).Val);
					}
				}
				// if none of the alphas is bigger than 0, no edge is written
				if (IsEdge) {
					FOut.PutStr(TStr::Fmt("%d,%d", EI.GetSrcNId(), EI.GetDstNId()));
					FOut.PutStr(Line);
					FOut.PutStr("\r\n");
				}
			}
			else
				FOut.PutStr(TStr::Fmt("%d,%d,1\r\n", EI.GetSrcNId(), EI.GetDstNId()));
		}
	}
}

void TNIBs::SaveGroundTruth(const TStr& OutFNm) {
	TFOut FOut(OutFNm);

	// write nodes to file
	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
		FOut.PutStr(TStr::Fmt("%d,%s\r\n", NI.GetKey().Val, NI.GetDat().Name.CStr()));
	}

	FOut.PutStr("\r\n");

	// write edges to file (not allowing self loops in the network)
	for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
		if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }

		// not allowing self loops in the Kronecker network
		if (EI.GetSrcNId() != EI.GetDstNId()) {
			if (EI().Len() > 0) {
				FOut.PutStr(TStr::Fmt("%d,%d,", EI.GetSrcNId(), EI.GetDstNId()));
				for (int i=0; i<EI().Len()-1; i++) { FOut.PutStr(TStr::Fmt("%f,%f,", EI().GetKey(i).Val, EI()[i].Val)); }
				FOut.PutStr(TStr::Fmt("%f,%f", EI().GetKey(EI().Len()-1).Val, EI()[EI().Len()-1].Val));
				FOut.PutStr("\r\n");
			}
			else
				FOut.PutStr(TStr::Fmt("%d,%d,1\r\n", EI.GetSrcNId(), EI.GetDstNId()));
		}
	}
}

void TNIBs::SaveGroundTruthPajek(const TStr& OutFNm, const double& Step) {
    TFOut FOut(OutFNm);

    FOut.PutStr(TStr::Fmt("*Vertices %d\r\n", NodeNmH.Len()));
    for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
      FOut.PutStr(TStr::Fmt("%d \"%s\" ic Blue\r\n", NI.GetKey().Val+1, NI.GetDat().Name.CStr()));
    }
    FOut.PutStr("*Arcs\r\n");
    for (TStrFltFltHNEDNet::TEdgeI EI = Network.BegEI(); EI < Network.EndEI(); EI++) {
    	if (!NodeNmH.IsKey(EI.GetSrcNId()) || !NodeNmH.IsKey(EI.GetDstNId())) { continue; }
    	double trueAlpha = 0.0;
    	if (EI().IsKey(Step)) { trueAlpha = EI().GetDat(Step); }
    	else { for (int j=0; j<EI().Len() && EI().GetKey(j)<=Step; j++) { trueAlpha = EI()[j]; } }

    	if (trueAlpha > MinAlpha) {
    		FOut.PutStr(TStr::Fmt("%d %d %f\r\n", EI.GetSrcNId()+1, EI.GetDstNId()+1, (trueAlpha > MaxAlpha? MaxAlpha.Val : trueAlpha)));
    	}
    }
}

void TNIBs::SaveSites(const TStr& OutFNm, const TIntFltVH& CascadesPerNode) {
	TFOut FOut(OutFNm);

	// write nodes to file
	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
		FOut.PutStr(TStr::Fmt("%d,%s", NI.GetKey().Val, NI.GetDat().Name.CStr()));
		if (CascadesPerNode.IsKey(NI.GetKey().Val)) {
			for (int i=0; i<CascadesPerNode.GetDat(NI.GetKey().Val).Len(); i++) {
				FOut.PutStr(TStr::Fmt(",%f", CascadesPerNode.GetDat(NI.GetKey().Val)[i].Val));
			}
		}
		FOut.PutStr("\r\n");
	}
}

void TNIBs::SaveCascades(const TStr& OutFNm) {
	TFOut FOut(OutFNm);

	// write nodes to file
	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++) {
		FOut.PutStr(TStr::Fmt("%d,%s\r\n", NI.GetKey().Val, NI.GetDat().Name.CStr()));
	}

	FOut.PutStr("\r\n");

	// write cascades to file
	for (THash<TInt, TCascade>::TIter CI = CascH.BegI(); CI < CascH.EndI(); CI++) {
		TCascade &C = CI.GetDat();
		int j = 0;
		for (THash<TInt, THitInfo>::TIter NI = C.NIdHitH.BegI(); NI < C.NIdHitH.EndI(); NI++) {
			if (!NodeNmH.IsKey(NI.GetDat().NId)) { continue; }
			if (j > 0) { FOut.PutStr(TStr::Fmt(",%d,%f", NI.GetDat().NId.Val, NI.GetDat().Tm.Val)); }
			else { FOut.PutStr(TStr::Fmt("%d;%d,%f", CI.GetKey().Val, NI.GetDat().NId.Val, NI.GetDat().Tm.Val)); }
			j++;
		}

		if (j >= 1)
			FOut.PutStr(TStr::Fmt("\r\n"));
	}
}
