
void Plot(double energy = 10, int rebin = -1, bool draw = true)
{

    TFile* file = TFile::Open(Form("neutron_%.1fMeV.root",energy));
    
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

    TCanvas * c1 = new TCanvas();
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

    TCanvas* c2 = new TCanvas();
    el->SetStats(0);
    el->Draw("h");
    nonel->Draw("h same");
    gPad->SetLogy();
    TLegend * l2 = new TLegend(0.6,0.7,0.89,0.89);
    l2->AddEntry(el,"elastic","l");
    l2->AddEntry(nonel,"inelastic","l");
    l2->Draw("same");

    double counts = el->GetEntries() + nonel->GetEntries();

    std::cout << " Neutron Energy = " << energy << " MeV " << "\n";
    std::cout << " Counts: \n";
    std::cout << " deuteron breakup : " << breakup->GetEntries() << "\t\t\t / total = " << double(breakup->GetEntries())/counts << "\n";
    std::cout << " non - deuteron breakup (inel) : " << non_breakup->GetEntries() << "\t / total = " << double(non_breakup->GetEntries())/counts << "\n";
    std::cout << " non - deuteron breakup (el) : " << non_breakup_el->GetEntries() << "\t / total = " << double(non_breakup_el->GetEntries())/counts << "\n";
    std::cout << "\t breakup reactions w/ Q == -2.224 MeV : " << breakup->GetBinContent(breakup->FindBin(-2.224)) << " \t / all breakup = " << double(breakup->GetBinContent(breakup->FindBin(-2.224)))/breakup->GetEntries() << "\t / total = " << double(breakup->GetBinContent(breakup->FindBin(-2.224)))/counts;    
    std::cout << "\n\t breakup reactions w/ Q != -2.224 MeV : " << breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(-2.224)) << " \t / all breakup = " << double(breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(-2.224)))/breakup->GetEntries() << "\t / total = " << double(breakup->GetEntries()-breakup->GetBinContent(breakup->FindBin(-2.224)))/counts << "\n";    

    std::cout << " elastic : " << el->GetEntries()   << "\t\t / total = " << double(el->GetEntries())/counts << "\n";
    std::cout << " inelastic : " << nonel->GetEntries() << "\t / total = " << double(nonel->GetEntries())/counts << "\n\n";
        
    gROOT->SetBatch(kFALSE);

}

void PlotAll(int rebin = -1, bool draw = false)
{
    
    Plot(2.5,rebin,draw);
    Plot(5  ,rebin,draw);
    Plot(7.5,rebin,draw);
    Plot(10 ,rebin,draw);

}
