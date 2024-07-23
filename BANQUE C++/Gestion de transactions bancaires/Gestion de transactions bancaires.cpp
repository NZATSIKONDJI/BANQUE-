#include <iostream>
#include <vector>

class Banque
{
private:
    const int id;
    std::string name;
    float solde = 0;

public:

    Banque(int id, std::string name) : id(id), name(name) {}

    ~Banque() {}

    float getSolde()
    {
        return solde;
    }

    float taxeBanque(float montant)
    {
        if (montant > 0)
        {
            if (montant * 0.1 < 10)
            {
                solde = montant * 0.1;
                return montant * 0.1;
            }
            else
            {
                solde = 10;
                return 10;
            }
        }
    }
};

class CompteBancaires
{
protected:
    float solde = 0;
    const int numeroCompte;

    CompteBancaires(float solde, int numeroCompte) : solde(solde), numeroCompte(numeroCompte){}

public:

    virtual ~CompteBancaires() { std::cout << "compte supprimé" << std::endl; }

    virtual void transfert(CompteBancaires* dest, float montantTransfert, Banque& banque) = 0;

    virtual const int getType() = 0; //recuperer les type de l'enfant, 1 si c'est un compte courant, 2 si c'est un livret

    float getSolde()
    {
        return solde;
    }

    int getNumeroCompte()
    {
        return numeroCompte;
    }

    void ajoutSolde(float montant)
    {
        if (montant > 0)
        {
            solde += montant;
        }
    }

};

class CompteCourant : public CompteBancaires
{
public:

    CompteCourant(float solde, int numeroCompte) : CompteBancaires(solde, numeroCompte) {}

    virtual ~CompteCourant() {}

    virtual void transfert(CompteBancaires *dest, float montantTransfert, Banque& banque) override
    {
        if (montantTransfert > 0)
        {
            if (montantTransfert <= solde)
            {
                if (dest->getType() == 2)
                {
                    solde -= montantTransfert;
                    float taxeBanque = banque.taxeBanque(montantTransfert);
                    dest->ajoutSolde(montantTransfert - taxeBanque);
                }
                else
                {
                    solde -= montantTransfert;
                    dest->ajoutSolde(montantTransfert);
                }
                
            }
            else
            {
                std::cout << "transfert impossible" << std::endl;
            }
        }
    }

    virtual const int getType() override { return 1;} 
};

class Livret : public CompteBancaires
{
private:
    float tauxInteret = 0.1;
    float sommeInteret = 0;

public:

    Livret(float solde, int numeroCompte, float _tauxInteret) : CompteBancaires(solde, numeroCompte) 
    {
        if (_tauxInteret <=1 && _tauxInteret >= 0)
        {
            tauxInteret = _tauxInteret;
        }
    
    }

    virtual ~Livret() {}

    void ajoutInteret() 
    {
        sommeInteret += solde * tauxInteret;
    }

    void ajoutInteretsSolde()
    {
        solde += sommeInteret;
    }

    virtual void transfert(CompteBancaires* dest, float montantTransfert, Banque& banque) override
    {
        if (montantTransfert > 0)
        {
            if (montantTransfert <= solde && dest->getType() == 1)
            {
                solde -= montantTransfert;
                float taxeBanque = banque.taxeBanque(montantTransfert);
                dest->ajoutSolde(montantTransfert-taxeBanque);
            }
            else
            {
                std::cout << "transfert impossible" << std::endl;
            }
        }
    }

    virtual const int getType() override { return 2; }
};

class Individu
{
private:
    const int id;
    std::string name;
    std::vector<CompteBancaires*> comptes;
    bool livret = false;

public:

    Individu(int id, std::string name, std::vector<CompteBancaires*> _comptes) : id(id), name(name)
    {
        comptes = _comptes;

        for (auto& compte : comptes)
        {
            if (livret && compte->getType() == 2)
            {
                std::cout << "un seul livret possible" << std::endl;
                auto index = std::find(comptes.begin(), comptes.end(), compte);
                comptes.erase(index);
                delete compte;

            }

            else if (compte->getType() == 2)
            {
                livret = true;
            }
        }
    }
        

    ~Individu() {}

    void ajoutCompte(CompteBancaires* compte)
    {
        if (compte->getType() == 2 && livret)
        {
            std::cout << "un seul livret possible" << std::endl;
            delete compte;
        }
        else
        {
            comptes.push_back(compte);
        }
    }

    void delCompte(int numeroCompte)
    {
        for (auto & compte : comptes)
        {
            if (compte->getNumeroCompte() == numeroCompte)
            {
                if (compte->getType() == 2)
                {
                    livret = false;
                }
                std::cout << "compte supprimé" << std::endl;
                auto index = std::find(comptes.begin(), comptes.end(), compte);
                comptes.erase(index);
                delete compte;
            }
        }
    }

    std::vector<CompteBancaires*> getComptes()
    {
        return comptes;
    }
};

float masseMonaitaire(Banque& banque, std::vector<Individu>& individus)
{
    float total = 0;
    total += banque.getSolde();

    for (Individu &individu : individus)
    {
        for (auto& elements : individu.getComptes())
        {
            total += elements->getSolde();
        }
    }
    return total;
}

int main()
{
    Banque banque = Banque(1, "karlBank");

    /*std::vector<CompteBancaires*> comptes;
    comptes.push_back(new CompteCourant(1200, 1));
    comptes.push_back(new CompteCourant(1200, 2));
    comptes.push_back(new CompteCourant(1200, 3));
    comptes.push_back(new Livret(1200, 1, 0.2));
    comptes.push_back(new Livret(1200, 2, 0.2));
    comptes.push_back(new Livret(1200, 3, 0.2));*/

    std::vector<Individu> individus;

    individus.push_back(Individu(1, "test", std::vector<CompteBancaires*>{ new CompteCourant(1200, 1), new CompteCourant(1200, 2), new Livret(1200, 1, 0.2), new Livret(1200, 2, 0.2) }));

    individus[0].getComptes()[0]->transfert(individus[0].getComptes()[2], 1100, banque);
    std::cout << individus[0].getComptes()[2]->getSolde() << std::endl;
    std::cout << banque.getSolde() << std::endl;
    

    float total = masseMonaitaire(banque, individus);

    for (auto& individu : individus)
    {
        for (auto& element : individu.getComptes())
        {
            delete element;
        }
    }

    std::cout << total;

    return 0;
}