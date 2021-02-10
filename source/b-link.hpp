// Copyright
#ifndef SOURCE_B_LINK_HPP_
#define SOURCE_B_LINK_HPP_

#include <utility>
using namespace std;

namespace EDA {

namespace Concurrent {
//////////////////////
template <std::size_t B, typename Type>

class BLinkTree {
    struct node {

        long ele[4];
        long child[4];
        node *next;

    };

 public:

    node *tree[10][10];
    int count[10];
    int leaf;
    int path[10];
    node *head;

  typedef Type data_type;

  BLinkTree() {

      leaf=-1;                                      //no leaf
      for(int i=0;i<10;i++)
      {
          count[i]=-1;                                                                                       //cont
          path[i]=-1;                                                                                        //para el cam
      }

  }

  std::size_t size() const {}

  bool empty() const {}

    node* create_node()
    {
        node* n;
        n=new node;                                                         //resv mem
        for(int i=0;i<4;i++)
        {
            n->ele[i]=-1;
            n->child[i]=-1;
        }
        n->next=NULL;
        return n;                                                           //r n
    }

    void search_2(int key)                               //b hoja
    {
        int i,j,temp;
        path[0]=0;
        if(leaf)
        {
            j=0;
            for(i=0;i<leaf;i++)
            {
                temp=search_node(tree[i][j],key);
                path[i+1]=temp;
                j=temp;
            }
        }
    }

    int search_node(node *node1,long key)
    {
        if(key<=node1->ele[0])                                  // caso cabeza
            return node1->child[0];
        for(int i=1;i<4;i++)
        {
            if((key>=node1->ele[i]) && (key<node1->ele[i+1]))
                return node1->child[i];
            else if(node1->ele[i+1]==-1)
                return node1->child[i];
        }
        return 0;
    }


    void search(const data_type& value) {
        int flag=0,                                                                                                                                            i;
        node *node1;                                                        //auz
        search_2(value);
        node1=tree[leaf][path[leaf]];
        for(i=0;node1->ele[i]!=-1;i++)
            if(node1->ele[i]==value)
            {
                flag=1;
                break;
            }
        std::cout<<"\n :";//
        for(i=0;path[i]!=-1;i++)
            std::cout<<path[i]<<"->";
        if(flag)
            std::cout<<"\nElemento encontrado\n";
        else
            std::cout<<"\nNo se encontro\n";
  }


  void insert_node(node *node1,int key)
    {
        int flag=0,count=-1,i,j,x,y,l;
        node *newnode,*parent;

        for(i=0;i<4;i++)
            if(node1->ele[i]!=-1)
                ++count;
        i=0;
        while(!flag&&node1->ele[i]!=-1)
        {
            if(node1->ele[i]>key)
            {
                flag=1;
                for(int j=count;j>=i;j--)
                    node1->ele[j+1]=node1->ele[j];
                node1->ele[i]=key;
            }
            i++;
        }
        if(!flag)
            node1->ele[count+1]=key;
        if(node1->ele[0]==key)
        {
            for(i=leaf-1;i>=0;i--)
            {
                x=path[i+1];
                if(tree[i][path[i]]->ele[x]>key)
                    tree[i][path[i]]->ele[x]=key;
                else
                    insert_node(tree[i][x],key);
            }
        }

        for(i=0;i<=count+1;i++)
            std::cout<<"\t\t"<<node1->ele[i];
    }


  void insert(const data_type& value) {
      int n,parent;
      node *first_node;
      if(leaf==-1)
      {
          first_node=create_node();
          tree[0][0]=first_node;
          leaf++;
          count[0]++;
          first_node->ele[0]=value;
          head=first_node;
      }
      else if(leaf==0)
      {
          if(nodefull(tree[0][0]))
          {
              path[leaf]=0;
              split(tree[0][0]);
              insert(value);
          }
          else
              insert_node(tree[0][0],value);
      }
      else
      {
          search(value);
          n=path[leaf];
          parent=path[leaf-1];
          if((nodefull(tree[leaf][n])))
          {
              split(tree[leaf][n]);
              insert(value);
          }
          else
              insert_node(tree[leaf][n],value);
      }
  }


  void remove(const data_type& value) {
      ///
  }

  int nodefull(node* node1)
    {
        if(node1->ele[3]!=-1) return 1;
        else     return 0;
    }


    void split(node *oldnode)
    {
        node *newnode,*parent,*n1,*n2;
        int i,j,k,n,t,x,y,pos;
        newnode=create_node();
        newnode->ele[0]=oldnode->ele[2];
        newnode->ele[1]=oldnode->ele[3];
        oldnode->ele[2]=-1;
        oldnode->ele[3]=-1;
        t=count[leaf];
        n=path[leaf];
        for(i=t,j=t+1;i>n;i--,j--)
            tree[leaf][j]=tree[leaf][i];
        newnode->next=tree[leaf][n]->next;
        tree[leaf][n]->next=newnode;
        tree[leaf][n+1]=newnode;
        count[leaf]++;
        x=leaf;
        if(count[leaf]+1==1) t=1;
        else
            t=log(count[leaf]+1)/log(2);
        if(t!=leaf)
        {
            ++leaf;
            count[leaf]=count[x];
            for(i=0;i<=count[leaf];i++)
                std::swap(tree[leaf][i],tree[x][i]);
        }
        for(i=leaf-1;i>=0;i--) count[i]=-1;
        for(i=t,j=i-1;i>0;i--,j--)
        {
            for(k=0;k<=(count[i])/3;k++)
            {
                n1=tree[i][2*k];
                n2=tree[i][(2*k)+1];
                newnode=create_node();
                count[j]++;
                tree[j][count[j]]=newnode;
                newnode->ele[0]=n1->ele[0];
                newnode->child[0]=2*k;
                newnode->ele[1]=n2->ele[0];
                newnode->child[1]=(2*k)+1;
            }
            if(count[i]!=1 && count[i]%2==0)
            {
                n2=tree[i][count[i]];
                for(y=0;n2->ele[y]!=-1;y++);
                newnode->ele[2]=n2->ele[0];
                newnode->child[2]=count[i];
            }
        }
    }

private:
  data_type* data_;
};

}  // namespace Concurrent
}  // namespace EDA

#endif  // SOURCE_B_LINK_HPP_
