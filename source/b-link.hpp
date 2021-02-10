// Copyright
#ifndef SOURCE_B_LINK_HPP_
#define SOURCE_B_LINK_HPP_

#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

template <class T>
bool comparacion(T* y,T* x)
{
    return *y<*x;
}

namespace EDA::Concurrent {
    template <size_t B,typename Type>

    class Nodo{
    public:

        bool Hoja;
        Nodo* Lista_linkz;
        int m;
        typedef Type data_type;
        mutex Nodo_mutex;
        vector<data_type*> clave;
        vector<Nodo*> ptr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Nodo()
        {
            m=B;
            Lista_linkz=nullptr;
            Hoja=true;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void insert(const data_type& value)
        {
            auto* p= new data_type;
            *p=value;
            clave.push_back(p);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <size_t B, typename Type>
    class BLinkTree {
    public:

        typedef Type data_type;
        mutex auxiliar_m;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BLinkTree()
        {
            root=NULL;
        }
        ~BLinkTree() {}
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        size_t size() const
        {
            return B;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool empty() const
        {
            if(!root||root->clave.empty())
                return true;
            return false;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void insert(const data_type& value)
        {
            data_type t2=value;
            vector<Nodo<B,data_type>*> kam;
            Nodo<B,data_type>* rt;
            search1(rt, kam, value);
            auxiliar_m.lock();
            if(root==NULL)
            {
                root= new Nodo<B,data_type>;
                root->insert(value);
                auxiliar_m.unlock();
                return;
            }
            auxiliar_m.unlock();

            for(int i=0;i<rt->clave.size();i++)
            {
                if(*rt->clave[i]==value)
                    return;
            }
            Nodo<B,data_type>* to_be;
            while(true)
            {
                unique_lock<mutex> w_lock(rt->Nodo_mutex);

                if(kam.empty()&&rt!=root)//overflow
                {
                    Nodo<B,data_type>* aux_Nodo=root;
                    while (aux_Nodo!=rt) {
                        if(rt->clave[0]>aux_Nodo->clave.back()&&aux_Nodo->Lista_linkz)
                            aux_Nodo=aux_Nodo->Lista_linkz;
                        else
                        {
                            int rn = aux_Nodo->clave.size();
                            for (int i = 0; i < rn; i++) {
                                if (*rt->clave[0] < *aux_Nodo->clave[i]) {
                                    kam.push_back(aux_Nodo);
                                    aux_Nodo = aux_Nodo->ptr[i];
                                    break;
                                }
                                if (i == rn - 1)
                                    kam.push_back(aux_Nodo);
                                if(!aux_Nodo->ptr.empty())
                                    aux_Nodo = aux_Nodo->ptr[i + 1];
                            }
                        }
                    }
                }

                while(t2>*rt->clave.back()&&rt->Lista_linkz)
                {
                    w_lock.unlock();
                    rt=rt->Lista_linkz;
                    w_lock.release();
                    w_lock=unique_lock<mutex>(rt->Nodo_mutex);
                }
                if(rt->clave.size()==(rt->m)-1)
                {
                    int goes_up=((rt->m)/2);
                    if(rt->m%2==0)
                        goes_up++;
                    auto* p= new data_type;
                    *p=t2;
                    rt->clave.push_back(p);

                    sort(rt->clave.begin(),rt->clave.end(),comparacion<data_type>);
                    if(rt->Hoja)
                    {
                        auto* temp = new Nodo<B,data_type>;
                        vector<data_type*> temp_1;
                        vector<data_type*> temp2;
                        int count_aux=goes_up;
                        t2=*rt->clave[goes_up];
                        while((rt->m)>count_aux)
                        {
                            temp_1.push_back(rt->clave[count_aux]);
                            count_aux++;
                        }
                        temp->clave=temp_1;
                        count_aux=0;
                        while(count_aux<goes_up)
                        {
                            temp2.push_back(rt->clave[count_aux]);
                            count_aux++;
                        }
                        rt->clave=temp2;
                        temp->Lista_linkz=rt->Lista_linkz;
                        rt->Lista_linkz=temp;
                        if(rt==root)
                        {
                            auto* temp_root = new Nodo<B,data_type>;
                            temp_root->insert(t2);
                            temp_root->ptr.push_back(rt);
                            temp_root->ptr.push_back(temp);
                            return;
                        }
                        to_be=temp;

                        if(!kam.empty())
                        {
                            rt=kam.back();
                            kam.pop_back();
                        }
                    }
                    else
                    {
                        int index;
                        for(int i=0;i<rt->clave.size();i++)
                        {
                            if(*rt->clave[i]==t2)
                                index=i;
                        }
                        vector<Nodo<B,data_type>*> pun_de;
                        for(int i=0;i<rt->ptr.size();i++)
                        {
                            if(i==index+1)
                                pun_de.push_back(to_be);
                            pun_de.push_back(rt->ptr[i]);
                        }

                        auto* temp = new Nodo<B,data_type>;
                        vector<data_type*> temp_1;
                        vector<data_type*> temp2;
                        int count_aux=goes_up;
                        t2=*rt->clave[goes_up];
                        temp->Hoja=false;
                        count_aux++;

                        while((rt->m)>count_aux)
                        {
                            temp_1.push_back(rt->clave[count_aux]);
                            count_aux++;
                        }
                        temp->clave=temp_1;
                        count_aux=0;

                        while(count_aux<goes_up)
                        {
                            temp2.push_back(rt->clave[count_aux]);
                            count_aux++;
                        }

                        rt->clave=temp2;
                        temp->Lista_linkz=rt->Lista_linkz;
                        rt->Lista_linkz=temp;
                        to_be=temp;

                        if(rt==root)
                        {
                            auto* temp_root = new Nodo<B,data_type>;
                            temp_root->insert(t2);
                            temp_root->ptr.push_back(rt);
                            temp_root->ptr.push_back(temp);

                            return;
                        }

                        rt=kam.back();
                        kam.pop_back();

                    }
                    w_lock.unlock();
                }
                else
                {
                    auto* p= new data_type;
                    *p=t2;
                    rt->clave.push_back(p);
                    sort(rt->clave.begin(),rt->clave.end(),comparacion<data_type>);

                    if(!rt->Hoja)
                    {
                        int index;
                        for(int i=0;i<rt->clave.size();i++)
                        {
                            if(*rt->clave[i]==t2)
                                index=i;
                        }
                        vector<Nodo<B,data_type>*> pun_de;
                        for(int i=0;i<rt->ptr.size();i++)
                        {
                            if(i==index+1)
                                pun_de.push_back(to_be);
                            pun_de.push_back(rt->ptr[i]);
                        }
                    }
                    w_lock.unlock();
                    return;
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool search(const data_type& value) const
        {
            if (empty())
                return false;
            Nodo<B,data_type>* rt;
            search2(rt,value);
            rt = root;
            for(int i=0;i<rt->clave.size();i++)
            {
                if(*rt->clave[i]==value)
                    return true;
            }
            return false;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        Nodo<B,data_type>* root;

        void search2(Nodo<B,data_type>* &rt, data_type value) const
        {
            rt = root;
            while (!(rt->Hoja)) {
                if(value>*rt->clave.back()&&rt->Lista_linkz)
                    rt=rt->Lista_linkz;
                else{
                    int rn = rt->clave.size();
                    for (int i = 0; i < rn; i++) {
                        if (value < *rt->clave[i]) {
                            rt = rt->ptr[i];
                            break;
                        }
                        if (i == rn - 1)
                            rt = rt->ptr[i + 1];
                    }
                }
            }
        }
        void search1(Nodo<B,data_type>* &rt, std::vector<Nodo<B,data_type>*> &kam, data_type value) const {
            rt = root;
            while (!rt->Hoja) {
                if(value>*rt->clave.back()&&rt->Lista_linkz)
                    rt=rt->Lista_linkz;
                else{
                    int rn = rt->clave.size();
                    for (int i = 0; i < rn; i++) {
                        if (value < *rt->clave[i]) {
                            kam.push_back(rt);
                            rt = rt->ptr[i];
                            break;
                        }
                        if (i == rn - 1)
                            kam.push_back(rt);
                        rt = rt->ptr[i + 1];
                    }
                }
            }
        }


    };


}  // namespace EDA
#endif  // SOURCE_B_LINK_HPP_
