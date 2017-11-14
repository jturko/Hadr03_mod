
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

    int nChannels = 19;
    TH1F ** h = (TH1F**)malloc(nChannels*sizeof(TH1F*));
    TH1F ** h_gamma = (TH1F**)malloc(nChannels*sizeof(TH1F*));
    for(int i=0; i<nChannels; ++i) {
        h[i] = new TH1F(Form("h%d",i),Form("h%d",i),4000,-20,20);
        h_gamma[i] = new TH1F(Form("h%d_gamma",i),Form("h%d_gamma",i),4000,-20,20);
    }

    TH2F * ch_q = new TH2F("ch_q","ch_q",21,-1,20,4000,-20,20);
    TH2F * ch_q_gamma = new TH2F("ch_q_gamma","ch_q_gamma",21,-1,20,4000,-20,20);

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
        h[channel]->Fill(val2);
        h_gamma[channel]->Fill(val);
        ch_q->Fill(channel,val2);
        ch_q_gamma->Fill(channel,val);
    }
    std::cout << std::endl;

}
