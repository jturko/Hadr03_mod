
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

    TH1F * h6 = new TH1F("h6","h6",300,-10,20);
    TH1F * h7 = new TH1F("h7","h7",300,-10,20);

    int size;
    double val;
    for(int i=0; i<entries; ++i) {
        if(i%10000==0) std::cout << " " << 100.*i/entries << " % done... \r";
        tree->GetEntry(i);
        size = int(pVector->size());
        val = -1.0*En;
        for(int j=0; j<size; ++j) {
            if(pVector->at(j) == 2) continue; // if one of the exiting particles is a gamma ray skip its energy
            val += eVector->at(j);
        }    
        if(channel == 6) h6->Fill(val);
        if(channel == 7) h7->Fill(val);
    }
    std::cout << std::endl;





}
