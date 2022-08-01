#include <vector>
#include <cstring>
#include <iostream>
#include <string.h>
#include <fstream>
#include <algorithm>

//#include "pilhaString.h"

using namespace std;
struct Node
{
    string ch;
    int freq;
    Node *left;
    Node *right;
};

class Huffman // Prefix-free code expansion (decoding)
{
    public:
        void codificaTexto(FILE* arq);
        void decodificaTexto(FILE* arq);
        void arvore(FILE*arq);
        void leArvore();
    private:
    	void inorder_write(ofstream &p, Node*folha);
        void add(Node key, Node*folha);
        void add(Node key);
        void constroiArvore(int ** letras, int tam);
        int ** insertionSortTabela(int **letras, int tam);

    Node * root;
    int ** vOrde;

};


int ** insertionSort(int ** array, int n)
{
    int j;
    int * temp = (int*)malloc(2*sizeof(int));

    for (int i = 0; i < n; ++i)
    {   
        j = i;
        while (j > 0 && array[j][0] < array[j-1][0])
        {
            temp[0] = array[j][0];
            temp[1] = array[j][1];
            array[j][0] = array[j-1][0];
            array[j][1] = array[j-1][1];
            array[j-1][0] = temp[0];
            array[j-1][1] = temp[1];
            j--;
        }
    }

    return array;
}

int ** Huffman::insertionSortTabela(int **letras, int tam)
{    
    return insertionSort(letras, tam);
}

void Huffman::constroiArvore(int ** letras, int tam)
{
    vector<Node> nos;
    root = NULL;

    for(int i = 0; i < tam; i++)
    {
        Node no;
        int inteiro = letras[i][1];
        int freq = letras[i][0];
        char teste = char(inteiro);
        string s(1, teste);

        no.ch = s;
        no.freq = freq;
        no.left = NULL;
        no.right = NULL;
        
        nos.push_back(no);
    }
    
    for(int i = 1; i < tam; i++)
    {
        add(nos.at(i));
    }    
}

void Huffman::add(Node key, Node*folha)
{                
    
   Node * pai = new Node;
   Node * filho = new Node;
   filho->ch = key.ch;
   filho->freq = key.freq;
   filho->left = key.left;
   filho->right = key.right;

   string aux = filho->ch;
   aux += root->ch;
   pai->ch = aux;
   pai->freq = filho->freq + folha->freq;
   pai->left = filho;
   pai->right = folha;
   root = pai; 

}

void Huffman::add(Node key)
{
	if(root != NULL){
		add(key, root);
	}else{ //caso não exista uma arvore um nó será criado e a freq será um
		root = new Node;
		root->ch = key.ch;
		root->left = key.left;
		root->right = key.right;
		root->freq = key.freq;
	}
}

void Huffman::arvore(FILE * arquivo)
{  
    
    root = NULL;
    int* palavras = (int*)malloc(128 * sizeof(int));
    for(int i = 0; i < 128; i++)
    {
        palavras[i] = 0;
    }

    int linhas = 0, espacos = 0, i = 0;
    char caractere;
    do {
        caractere = getc(arquivo);
        int ascii = int(caractere);
        if(palavras[ascii] == NULL)
            palavras[ascii] = 1;
        else
            palavras[ascii] = palavras[ascii] + 1;

    } while(caractere != EOF);
    //printf("O arquivo possui %d espacos e %d linhas.\n", espacos, linhas);

   

    int tam = 0;
    for(int i = 0; i < 128; i++)
    {
        if(palavras[i] != 0)
            tam++;
    }

    vOrde = (int**)malloc(tam * sizeof(int*));

    int index = 0;
    for(int i = 0; i < 128; i++)
    {
        if(palavras[i] != 0)
        {
            vOrde[index] = (int*)malloc(2 * sizeof(int));
            vOrde[index][0] = palavras[i];
            vOrde[index][1] = i;
            index++;            
        }
    }

    vOrde = insertionSortTabela(vOrde, tam);

    constroiArvore(vOrde, tam);
    ofstream p;
    p.open ("arvore.txt");

    inorder_write(p, root);
    p.close();
    
    
}

void Huffman::codificaTexto(FILE *arquivo)
{
    //FILE * q = fopen("abobora.txt", "r");
    FILE * p = fopen("compactado.bin", "wb");
    if(p == NULL)
        cout << "ERRO: O arquivo.bin não foi aberto";

    Node * auxN = NULL;
    string auxS;

    int zero = '0';
    int um = '1';
    
    char caractere;
    do {
        caractere = getc(arquivo);
        string v(1, caractere);
        auxN = root;
        while(true)
        {          
            if(auxN->left == NULL && auxN->right == NULL)
                break;

            auxS = auxN->ch.substr(0,1);

            if(auxS == v)
            {
                fwrite(&zero, sizeof(char), 1, p);
                auxN = auxN->left;
            }
            else if(auxS != v)
            {
                fwrite(&um, sizeof(char), 1, p);
                auxN = auxN->right;
            }
        }
    } while(caractere != EOF);

    fclose(p);
}

void Huffman::decodificaTexto(FILE* p)
{
    vector<char> bin;

    int linhas = 0, espacos = 0, i = 0;
    char caractere;
    do {
        caractere = getc(p);
        
        bin.push_back(caractere);
        i++;

    } while(caractere != EOF);
    //printf("O arquivo possui %d espacos e %d linhas.\n", espacos, linhas);

    int tam = bin.size();
    char* binario = (char*)malloc(tam * sizeof(char));
    for(int i = 0; i < tam; i++)
    {
        binario[i] = bin.at(i);
    }
    fclose(p);

    Node *auxN;
    ofstream myfile;
    myfile.open ("descompactado.txt");

    auxN = root;
    for(int i = 0; i < tam-5; i++)
    {

        if(binario[i] == '0')
        {
            auxN = auxN->left;
        }
        else if(binario[i] == '1')
        {
            auxN = auxN->right;
        }

        if(auxN->left == NULL && auxN->right == NULL)
        {
            myfile << auxN->ch;
            auxN = root;
        }
    
    }
    myfile.close();
}

void Huffman::inorder_write(ofstream &p, Node*folha){
	if(folha != NULL){
		inorder_write(p, folha->left);

        p << folha->ch;
        p << "|";
		inorder_write(p, folha->right);
	}
}

void Huffman::leArvore()
{
    vector<string> nos;
    FILE *p = fopen("arvore.txt", "r");
    if(p == NULL)
        cout << "ERRO: O arquivo não foi aberto!" << endl;

    string aux;
    char caractere;
    do {
        caractere = getc(p);
        if(caractere != '|')
        {
            aux += caractere;
        }else
        {
            nos.push_back(aux);
            aux = "";
        }

    } while(caractere != EOF);

    int tam = nos.size();

    vector<Node> vetorNos;

    for(int i = 0; i < tam; i++)
    {
        Node no;
        no.ch = nos.at(i);
        no.left = NULL;
        no.right = NULL;
        vetorNos.push_back(no);
    }

    root = NULL;
    for(int i = tam-1; i > -1; i = i-2)
    {
        add(vetorNos.at(i));
    }    
    
}

int main(){
    

    char opcao;
    cout << "1- Compactar" << endl;
    cout << "2- Descompactar" << endl;
    cin >> opcao;

    if(opcao == '1')
    {
        char nome[50];
        cout << "Digite o nome do arquivo: ";
        scanf("%s", nome);
        FILE * arq = fopen(nome, "r");

        Huffman h;
        h.arvore(arq);
        fclose(arq);
        arq = fopen(nome, "r");
        h.codificaTexto(arq);
    }else{
        if(opcao == '2'){
            char nome[50];
            cout << "Digite o nome do arquivo: ";
            scanf("%s", nome);

            Huffman h;
            h.leArvore();
            FILE*p = fopen(nome, "r");
            h.decodificaTexto(p);
        }
    }
    
            
    return 0;
}
