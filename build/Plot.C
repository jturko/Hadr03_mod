
double * Plot(double energy = 10, int rebin = -1, double Qval = -2.22435, bool draw = true)
{

    TFile* file = TFile::Open(Form("data/neutron_%.1fMeV.root",energy));
    
    TH1F* breakup = (TH1F*)file->Get("13");
    TH1F* non_breakup = (TH1F*)file->Get("14");
    TH1F* non_breakup_el = (TH1F*)file->Get("15");
    breakup->SetLineColor(kBlack);
    non_breakup->SetLineColor(kRed);
    non_breakup_el->SetLineColor(kGreen);

    TH1F* el = (TH1F*)file->Get("16");
    TH1F* nonel = (TH1F*)file->Get("17");
    el->SetLineColor(kBlack);
    nonel->SetLineColor(kRed);
    
    if(rebin > 0) {
        breakup->Rebin(rebin);
        non_breakup->Rebin(rebin);
        non_breakup_el->Rebin(rebin);
        el->Rebin(rebin);
        nonel->Rebin(rebin);
    }
    if(!draw) {
        gROOT->SetBatch(kTRUE);
    }

    //TCanvas * c1 = new TCanvas();
    TCanvas * c1 = NULL;    
    if(gROOT->GetListOfCanvases()->GetSize() < 1) c1 = new TCanvas();   
    else                                          c1 = (TCanvas*)gROOT->GetListOfCanvases()->At(0);
    c1->cd();
    breakup->SetStats(0);
    breakup->Draw("h");
    non_breakup->Draw("h same");
    non_breakup_el->Draw("h same");
    gPad->SetLogy();
    TLegend * l1 = new TLegend(0.6,0.7,0.89,0.89);
    l1->AddEntry(breakup,"deuteron breakup","l");
    l1->AddEntry(non_breakup,"non deuteron breakup (inel)","l");
    l1->AddEntry(non_breakup_el,"non deuteron breakup (el)","l");
    l1->Draw("same");

    //TCanvas* c2 = new TCanvas();
    TCanvas * c2 = NULL;    
    if(gROOT->GetListOfCanvases()->GetSize() < 2) c2 = new TCanvas();   
    else                                          c2 = (TCanvas*)gROOT->GetListOfCanvases()->At(1);
    c2->cd();
    el->SetStats(0);
    el->Draw("h");
    nonel->Draw("h same");
    gPad->SetLogy();
    TLegend * l2 = new TLegend(0.6,0.7,0.89,0.89);
    l2->AddEntry(el,"elastic","l");
    l2->AddEntry(nonel,"inelastic","l");
    l2->Draw("same");

    double counts = el->GetEntries() + nonel->GetEntries();
    double Q = Qval;

    if(draw) {
        std::cout << " Neutron Energy = " << energy << " MeV \t Bin width = " << breakup->GetBinWidth(0) << " MeV";
        std::cout << " (from " << breakup->GetBinLowEdge(breakup->FindBin(Q)) << " to " 
                  << (breakup->GetBinLowEdge(breakup->FindBin(Q))+breakup->GetBinWidth(0)) << " MeV)\n"; 
        std::cout << " Counts: \n";
        std::cout << " deuteron breakup : " << breakup->GetEntries() << "\t\t\t / total = " 
                  << double(breakup->GetEntries())/counts << "\n";
        std::cout << " non - deuteron breakup (inel) : " << non_breakup->GetEntries() << "\t / total = " 
                  << double(non_breakup->GetEntries())/counts << "\n";
        std::cout << " non - deuteron breakup (el) : " << non_breakup_el->GetEntries() << "\t / total = " 
                  << double(non_breakup_el->GetEntries())/counts << "\n";
        std::cout << "\t breakup reactions w/ Q == " << Q << " MeV : " << breakup->GetBinContent(breakup->FindBin(Q)) 
                  << " \t / all breakup = " << double(breakup->GetBinContent(breakup->FindBin(Q)))/breakup->GetEntries() 
                  << "\t / total = " << double(breakup->GetBinContent(breakup->FindBin(Q)))/counts;    
        std::cout << "\n\t breakup reactions w/ Q != " << Q << " MeV : " 
                  << breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(Q)) << " \t / all breakup = " 
                  << double(breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(Q)))/breakup->GetEntries() 
                  << "\t / total = " << double(breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(Q)))/counts << "\n";    
        std::cout << " elastic : " << el->GetEntries()   << "\t\t / total = " << double(el->GetEntries())/counts << "\n";
        std::cout << " inelastic : " << nonel->GetEntries() << "\t / total = " << double(nonel->GetEntries())/counts << "\n\n";
    }        

    gROOT->SetBatch(kFALSE);
    
    double * ratios = new double[5];
    //ratios[0] = double(breakup->GetEntries())/counts; // fraction that are d-breakup
    //ratios[1] = double(non_breakup->GetEntries())/counts; // fraction that are NOT d-breakup, inelastic
    //ratios[2] = double(non_breakup_el->GetEntries())/counts; // fraction that are NOT d-breakup, elastic
    //ratios[3] = double(breakup->GetBinContent(breakup->FindBin(Q)))/breakup->GetEntries(); // fraction of d-breakup that conserve E
    //ratios[4] = double(breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(Q)))/breakup->GetEntries(); // fraction of d-breakup that DONT conserve E
    //ratios[3] = double(breakup->GetBinContent(breakup->FindBin(Q)))/counts; // fraction of total that are d-breakup that DO conserve E
    //ratios[6] = double(breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(Q)))/counts; // fraction of total that are d-breakup that DO NOT conserve E
    
    ratios[0] = double(breakup->GetEntries())/counts; // fraction that are d-breakup
    ratios[1] = double(breakup->GetBinContent(breakup->FindBin(Q)))/breakup->GetEntries(); // fraction of d-breakup that conserve E
    ratios[2] = double(breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(Q)))/breakup->GetEntries(); // fraction of d-breakup that DONT conserve E
    ratios[3] = double(breakup->GetBinContent(breakup->FindBin(Q)))/counts; // fraction of total that are d-breakup that DO conserve E
    ratios[4] = double(breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(Q)))/counts; // fraction of total that are d-breakup that DO NOT conserve E

    return ratios;

}

TGraph ** Calculate(int rebin = -1, double Q = -2.22435, bool draw = false)
{
    
    //TGraph * all_dbreak = new Graph();
    //TGraph * good_dbreak = new TGraph();
    //TGraph * bad dbreak = new TGraph();
    //TGraph * good_dbreak_all = new TGraph();
    //TGraph * bad_dbreak_all = new TGraph();
    
    //TGraph ** graphs;
    //(*graphs) = new TGraph[5];
    TGraph ** graphs = (TGraph**)malloc(5*sizeof(TGraph*));
    for(int i=0; i<5; i++) graphs[i] = new TGraph();
    
    double * data;
    double E;
    for(int i=0; i<199; i++) {
        E = (i+1)*0.1;
        data = Plot(E,rebin,Q,draw);
        for(int j=0; j<5; j++) {
            if(data[j] == data[j]) { 
                graphs[j]->SetPoint(i,E,data[j]);
            }
            else {
                graphs[j]->SetPoint(i,E,0);
            }
        }   
    }
    
    return graphs;
}

void PlotGraphs(int rebin = -1, double Q = -2.22435, bool draw = false) 
{
    TFile * f = TFile::Open("data/neutron_0.1MeV.root");
    TH1F * tmp = (TH1F*)f->Get("13");
    if(rebin > 0) tmp->Rebin(rebin);
    double binWidth = tmp->GetBinWidth(0);
    double lowEdge = tmp->GetBinLowEdge(tmp->FindBin(Q));
    std::cout << "bin witdh = " << binWidth << " MeV (from " << lowEdge << " to " << (lowEdge+binWidth) << " MeV)\n";

    gROOT->SetBatch(kFALSE);
    TGraph ** graphs = Calculate(rebin,Q,draw);
    //TCanvas * c1 = new TCanvas("c1");
    TCanvas * c1 = NULL;    
    if(gROOT->GetListOfCanvases()->GetSize() < 1) c1 = new TCanvas();   
    else                                          c1 = (TCanvas*)gROOT->GetListOfCanvases()->At(0);
    c1->cd();
    gPad->SetLogy(false);
    graphs[0]->SetMarkerStyle(20);
    graphs[3]->SetMarkerStyle(21);
    graphs[4]->SetMarkerStyle(22);
    graphs[3]->SetMarkerColor(2);
    graphs[4]->SetMarkerColor(3);
    graphs[3]->SetLineColor(2);
    graphs[4]->SetLineColor(3);
    graphs[0]->Draw("a l");
    graphs[3]->Draw("same l");
    graphs[4]->Draw("same l");

    TLegend * l1 = new TLegend(0.11,0.7,0.5,0.89);
    l1->AddEntry(graphs[0],"d breakup (total)","l");
    l1->AddEntry(graphs[3],"d breakup (good)","l");
    l1->AddEntry(graphs[4],"d breakup (bad)","l");
    l1->Draw("same");

    //TCanvas * c2 = new TCanvas("c2");
    TCanvas * c2 = NULL;    
    if(gROOT->GetListOfCanvases()->GetSize() < 2) c2 = new TCanvas();   
    else                                          c2 = (TCanvas*)gROOT->GetListOfCanvases()->At(1);
    c2->cd();
    gPad->SetLogy(false);
    graphs[1]->SetMarkerStyle(20);
    graphs[2]->SetMarkerStyle(21);
    graphs[1]->SetMarkerColor(2);
    graphs[2]->SetMarkerColor(3);
    graphs[1]->SetLineColor(2);
    graphs[2]->SetLineColor(3);
    graphs[2]->GetYaxis()->SetRangeUser(0,1.05);
    graphs[2]->Draw("a l");
    graphs[1]->Draw("l same");
    
    TLegend * l2 = new TLegend(0.5,0.7,0.89,0.89);
    l2->AddEntry(graphs[1],"d breakup (good)","l");
    l2->AddEntry(graphs[2],"d breakup (bad)","l");
    l2->Draw("same");
}
