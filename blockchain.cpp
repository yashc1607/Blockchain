#include<iostream>
#include<map>
#include<string>
#include<fstream>
#include "sha256.h"

using namespace std;

struct node{
    string data;
    node *left;
    node *right;
};
struct merkletree{
    node *tree;
};
struct chain{
    int block_number;
    merkletree *merkle_root;
    string block_hash;
    string transaction;
    chain *next;
    string prev_root;
    string validTransactionList;
};
struct transaction{
    string to;
    string from;
    int amount;
    int incentive;
};


int main(){
    int acctno,trno;
    ifstream myfile;
    myfile.open("input.txt");

    myfile>>acctno;  
    map<string,int>account;    
    for(int i=0;i<acctno;i++){
        string acctname;
        int amount;
        myfile>>acctname>>amount;
        account[acctname]=amount;
    }
    myfile>>trno;
    transaction *trs = new transaction[trno];
    for(int i=0;i<trno;i++){
        myfile>>trs[i].from>>trs[i].to>>trs[i].amount>>trs[i].incentive;
    }
    int i=0;
    struct chain *head=NULL,*tail=NULL;
    struct transaction *ptr1=trs;
    struct transaction *temp=new transaction;
    for(int i=0;i<trno-1;i++){
        for(int j=i;j<trno-1;j++){
            if((ptr1[j].incentive<ptr1[j+1].incentive)||((ptr1[j].incentive==ptr1[j+1].incentive)&&(ptr1[j+1].from<ptr1[j].from))){ //|
                temp->from=ptr1[j].from;
                temp->to=ptr1[j].to;
                temp->amount=ptr1[j].amount;
                temp->incentive=ptr1[j].incentive;

                ptr1[j].from=ptr1[j+1].from;
                ptr1[j].to=ptr1[j+1].to;
                ptr1[j].amount=ptr1[j+1].amount;
                ptr1[j].incentive=ptr1[j+1].incentive;

                ptr1[j+1].from=temp->from;
                ptr1[j+1].to=temp->to;
                ptr1[j+1].amount=temp->amount;
                ptr1[j+1].incentive=temp->incentive;    
            }
        }
        //cout<<ptr1[i].from<<" "<<ptr1[i].to<<" "<<ptr1[i].amount<<" "<<ptr1[i].incentive<<endl;
    }
    // for(int i=0;i<trno;i++){
    //     cout<<ptr1[i].from<<" "<<ptr1[i].to<<" "<<ptr1[i].amount<<" "<<ptr1[i].incentive<<endl;
    // }
    int flag=0;
    int blockCount=1;
    while(i<trno){
        struct chain *block=new chain;
        block->block_number=blockCount;
        block->next=NULL;
        block->merkle_root=NULL;
        block->prev_root="0";
        int trCount=0;
        struct merkletree *intermediateRoot1=new merkletree;
        string validTrnsList="[";
        while(trCount<2 && i<trno){
            string from=trs[i].from;
            int amt=trs[i].amount;
            int balance = account.find(from)->second;
            string inputstr;
            if(balance>=amt){
                struct node *newnode=new node;
                flag=1;
                if(newnode==NULL)
                inputstr=trs[i].from+trs[i].to+to_string(trs[i].amount);
                newnode->data=sha256(trs[i].from+to_string(trs[i].incentive)+trs[i].to+to_string(trs[i].amount));  
                //cout<<trs[i].from<<to_string(trs[i].incentive)<<trs[i].to<<to_string(trs[i].amount)<<endl;
                //cout<<trCount<<":"<<newnode->data<<endl;
                newnode->left=NULL;
                newnode->right=NULL;
                trCount++;
                if(trCount==1){
                    intermediateRoot1->tree=newnode;
                }
                else if(trCount==2){
                    struct node *root=new node;
                    root->left=intermediateRoot1->tree;
                    root->right=newnode;
                    root->data=sha256(root->left->data+root->right->data);
                    intermediateRoot1->tree=root;
                }
                balance=balance-amt;
                account[from]=balance;
                string to=trs[i].to;
                int rcvBal=account.find(to)->second;
                rcvBal+=amt;
                account[to]=rcvBal;
                if(validTrnsList!="["){
                    validTrnsList=validTrnsList+", ";
                }
                validTrnsList=validTrnsList+"['"+trs[i].from+"', '"+trs[i].to+"', "+to_string(trs[i].amount)+", "+to_string(trs[i].incentive)+"]";
                
            }
            i++;
        }
        
        if(flag==1){
            block->merkle_root=intermediateRoot1;
            block->validTransactionList=validTrnsList+"]";
        }
        
        if(trCount==2 && i<trno){
            struct merkletree *intermediateRoot2=new merkletree;
            while(trCount<4 && i<trno){ 
                string from=trs[i].from;
                int amt=trs[i].amount;
                int balance = account.find(from)->second;
                if(balance>=amt){
                    flag=1;
                    struct node *newnode1=new node;
                    newnode1->data=sha256(trs[i].from+to_string(trs[i].incentive)+trs[i].to+to_string(trs[i].amount));   
                    //cout<<trCount<<":"<<newnode1->data<<endl;
                    newnode1->left=NULL;
                    newnode1->right=NULL;
                    trCount++;
                    
                    if(trCount==3){
                        intermediateRoot2->tree=newnode1;
                    }
                    else if(trCount==4){
                        struct node *root1=new node;
                        root1->left=intermediateRoot2->tree;
                        root1->right=newnode1;
                        root1->data=sha256(root1->left->data+root1->right->data);
                        intermediateRoot2->tree=root1;
                    }
                    balance=balance-amt;
                    account[from]=balance;
                    string to=trs[i].to;
                    int rcvBal=account.find(to)->second;
                    rcvBal+=amt;
                    account[to]=rcvBal;
                    if(validTrnsList!="["){
                        validTrnsList=validTrnsList+", ";
                    }
                    validTrnsList=validTrnsList+"['"+trs[i].from+"', '"+trs[i].to+"', "+to_string(trs[i].amount)+", "+to_string(trs[i].incentive)+"]";
                }
                i++;
            }
            struct node *root2=new node;
            root2->left=intermediateRoot1->tree;
            root2->right=intermediateRoot2->tree;
            root2->data=sha256(root2->left->data+root2->right->data);
            //cout<<"result:"<<root2->data<<endl;
            intermediateRoot1->tree=root2;
            block->merkle_root=intermediateRoot1;
            block->validTransactionList=validTrnsList+"]";
        }
        if(flag==1){
            block->block_hash=sha256(block->prev_root+to_string(block->block_number)+block->merkle_root->tree->data);
            blockCount++;
            if(head==NULL && tail ==NULL){
                head=block;
                tail=block;
            }
            else{
                tail->next=block;
                block->prev_root=tail->block_hash;
                tail=tail->next;
                block->block_hash=sha256(block->prev_root+to_string(block->block_number)+block->merkle_root->tree->data);
            }
        }
    }
    if(flag==1){
        struct chain *ptr=head;
        while(ptr!=NULL){
            cout<<ptr->block_number<<endl;
            cout<<ptr->block_hash<<endl;
            cout<<ptr->validTransactionList<<endl;
            cout<<ptr->merkle_root->tree->data<<endl;
            ptr=ptr->next;
        }
    }
    else{
        cout<<"-1\n";
    }
   cout<<"\n\n"; 
    
    for (auto i = account.begin(); i != account.end(); i++) 
        cout << i->first << " " << i->second << endl; 
}
