
void AnalyzeTree(double En = 10.)
{

    TFile * f = TFile::Open("tree.root");
    TTree * tree = (TTree*)f->Get("tree");

    int entries = tree->GetEntries();
    std::cout << "tree has " << entries << " entries" << std::endl; 
   
    int channel;
    double Q;
    std::vector<int> * pVector = NULL;
    std::vector<double> * eVector = NULL;

    tree->SetBranchAddress("Channel",&channel);
    tree->SetBranchAddress("Q",&Q);
    tree->SetBranchAddress("ParticleVector",&pVector);
    tree->SetBranchAddress("EnergyVector",&eVector);

    TH1F * h6 = new TH1F("h6","h6",3000,-10,20);
    TH1F * h7 = new TH1F("h7","h7",3000,-10,20);
    TH1F * h7_gamma = new TH1F("h7_gamma","h7_gamma",3000,-10,20);
    TH2F * ch_q = new TH2F("ch_q","ch_q",21,-1,20,3000,-10,20);

    int size;
    double val, val2;
    for(int i=0; i<entries; ++i) {
        if(i%100000==0) std::cout << " " << 100.*i/entries << " % done... \r" << std::flush;
        tree->GetEntry(i);
        size = int(pVector->size());
        val = -1.0*En; // this is used for the q value WITHOUT gammas
        val2 = val; // this will be used for the q value WITH gammas
        for(int j=0; j<size; ++j) {
            if(pVector->at(j) != 2) val += eVector->at(j);
            val2 += eVector->at(j);
        }    
        if(channel == 6) {
            h6->Fill(val);
        }
        if(channel == 7) { 
            h7->Fill(val);
            h7_gamma->Fill(val2);
        }
        ch_q->Fill(channel,Q);
    }
    std::cout << std::endl;

}
