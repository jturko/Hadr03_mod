
void AnalyzeTree(double En = 10.)
{

    TFile * f = TFile::Open("tree.root");
    TTree * tree = (TTree*)f->Get("tree");
    
    std::string fname = "outfile.root";
    TFile * outfile = TFile::Open(fname.c_str(),"RECREATE");

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

    TH2F * ch_q = new TH2F("ch_q","ch_q",21,-1,20,4000,-20,20);
    TH2F * ch_q_gamma = new TH2F("ch_q_nogamma","ch_q_nogamma",21,-1,20,4000,-20,20);
    const int nParticles = 10;
    TH2F ** ch_e = (TH2F**)malloc(nParticles*sizeof(TH2F*));
    for(int i=0; i<nParticles; i++) ch_e[i] = new TH2F(Form("ch_e_%d",i),Form("ch_e_%d",i),21,-1,20,2500,0,25);

    int size;
    double val, val2;
    double energies[nParticles] = {0};
    for(int i=0; i<entries; ++i) {
        std::memset(energies,0,sizeof(energies));
        if(i%100000==0) std::cout << " " << 100.*i/entries << " % done... \r" << std::flush;
        tree->GetEntry(i);
        size = int(pVector->size());
        val = -1.0*En; // this is used for the q value WITHOUT gammas
        val2 = val; // this will be used for the q value WITH gammas
        for(int j=0; j<size; ++j) {
            if(pVector->at(j) != 2) val += eVector->at(j);
            val2 += eVector->at(j);
            energies[pVector->at(j)] += eVector->at(j);
            if(pVector->at(j) >= 0) ch_e[pVector->at(j)]->Fill(channel,eVector->at(j)); // if we want the NON summed energies of each particle type
        }
        ch_q->Fill(channel,val2);
        ch_q_gamma->Fill(channel,val);
        //for(int j=0; j<nParticles; ++j) ch_e[j]->Fill(channel,energies[j]); // if we want the summed energies of each particle type
    }
    std::cout << std::endl;

    std::cout << "saving to " << fname << "...";
    outfile->Write();
    std::cout << " done!" << std::endl;





}
