#include "btree.h"

BTree::BTree()
{
    height=0;
    read =0;
    write =1;
    root.currSize =0;
}

//Using a existing file create a BTree
void BTree::writeHeader(char * fileName)
{
    strcpy(treeFileName,fileName);
    treeFile.open(treeFileName,ios::in|ios::out|ios::binary|ios::trunc);
    treeFile.seekg(0,ios::beg);
    BTNode header;
    treeFile.read((char *) &header, sizeof(BTNode));
    read++;
    rootAddr = header.child[0];
    root = getNode(rootAddr);

}
//Call private insert method
void BTree::insert (keyType key)
{
    insert(key, rootAddr, -1, -1);
}

//Create a completely new BTree without an existing root from the file argument
void BTree::reset (char * fileName)
{
    //Initial variables set    
    height=0;
    read =0;
    write =1;

    strcpy(treeFileName,fileName);
    treeFile.open(treeFileName,ios::in|ios::out|ios::binary);
    treeFile.seekg(0,ios::beg);
    BTNode header;
    header.child[0] = sizeof(BTNode);
    treeFile.write((char *) &header, sizeof(BTNode));
    write++;
    root.currSize = 0;
    //Set all children to -1
    for(int i=0; i<5;i++)
    {
        root.child[i] = -1;
    }
    rootAddr = sizeof(BTNode);
    treeFile.write((char *) &root, sizeof(root));
    write++;
}

void BTree::close()
{
    treeFile.close();
}

void BTree::printTree()
{
    printTree(rootAddr);
}

void BTree::inorder()
{
    inorder(rootAddr);
}

void BTree::reverse()
{
    reverse(rootAddr);
}

int BTree::getHeight()
{
    return height;
}

bool BTree::search (string key)
{
    //BTNode t = getNode(rootAddr);
    //return search(key,t,rootAddr);
    return(key,root,rootAddr);
}

keyType BTree::retrieve (string key)
{
    //Check if root is a leaf
    if(search(key) != false)
    {
        BTNode t = getNode(rootAddr);

        for(int i=0; i< t.currSize;i++)
        {
            if(strcmp(key.c_str(), t.contents[i].getUPC().c_str()) ==  0)
            {
                return t.contents[i];
            }
        }
    }
    //This return should never be reached
    keyType dummy;
    return dummy;
}

void BTree::totalio() const //Print all read and writes
{
    cout << "Number of Reads: " << read <<endl;
    cout << "Number of Writes: " << write <<endl;
}


//////////////////////////////////PRIVATE METHODS////////////////////////////////////////////////////////////////

void BTree::printNode (int rootAddr)
{   
    //Check if root node exists
    if(rootAddr != -1)
    {
        BTNode t = getNode(rootAddr);
        for(int i=0; i<t.currSize;i++)
        {
            cout << t.contents[i] << endl;            
        }
    }
}

void BTree::inorder (int rootAddr)
{
    //Check if root exists
    if(rootAddr != -1)
    {
        BTNode t = getNode(rootAddr);

        //Recursively loop through and print the child of each individual branch
        for(int i=0; i< t.currSize;i++)
        {
            inorder(t.child[i]);
            cout << t.contents[i] << endl;            
        }
        inorder(t.child[t.currSize]);
    }

}
void BTree::reverse (int rootAddr)
{
    //Check if root exists
    if(rootAddr != -1)
    {
        BTNode t = getNode(rootAddr);
        //Recusively call to print through tree, but start at the leaves and make your way back up to root
        reverse(t.child[t.currSize]);
        for(int i=t.currSize-1; i>= 0; i--)
        {
            cout << t.contents[i] <<endl;
            reverse(t.child[i]);
        }
    }
}
//I ended up not using the regular findAddr
int BTree::findAddr (keyType key, BTNode t, int tAddr)
{
    if(isLeaf(t) == true)
        return tAddr;

    for(int i=0; i< t.currSize;i++)
    {
        if(key < t.contents[i])
        {

        }
    }
    return -1;
}

int BTree::findpAddr(keyType key, BTNode t, int tAddr, int cAddr)
{
    if( cAddr == rootAddr) 
        return -1;
    
    //check to see if the parent address is the current child address we're at
    for(int i=0;i <= t.currSize;i++)
    {
        if(t.child[i] == cAddr)
        {
            return tAddr;
        }
    }
    //Compare your key to all contents except the last right address
    for(int i=0; i< t.currSize;i++)
    {
        if(key < t.contents[i] && t.child[i] != -1)
        {
            return findpAddr(key, getNode(t.child[i]), t.child[i], cAddr );
        }
    }
    //Check last right address at currSize
    if(t.child[t.currSize] != -1)
    {
        return findpAddr(key, getNode(t.child[t.currSize]), t.child[t.currSize], cAddr);
    }

    //Please dont reach this, i beg of thee
    return -5555555;
}

void BTree::insert (keyType key, int recAddr, int oneAddr, int twoAddr)
{
    BTNode t = getNode(recAddr);

    //If this is the first insert
    if(root.currSize == 0)
    {
        cout<<"First Insert here"<<endl;
        root.contents[0] = key;
        root.currSize++;
        treeFile.seekg(recAddr,ios::beg);
        treeFile.write((char *) &root, sizeof(BTNode));
        write++;

    }
    else if(!isLeaf(recAddr))
    {
        //If youre not at a leaf compare key to all content except the last branch
        for(int i = 0; i< t.currSize; i++)
        {
            if(key < t.contents[i])
            {
                insert(key ,t.child[i] ,oneAddr, twoAddr);
                return;
            }
        }
        insert(key, t.child[t.currSize], oneAddr, twoAddr);
        return;
    }
    //Check and compare the last branch to see where key fits
    else if(t.currSize < ORDER -1)
    {
        int i;
        //Slide contents to the left
        for(i =t.currSize -1; i>=0; i--)
        {
            if(key < t.contents[i])
            {
                t.contents[i+1] = t.contents[i];
            }
            else
                break;
        }
        //Insert key into current position
        t.contents[i+1] = key;
        t.currSize++;

        treeFile.seekg(recAddr,ios::beg);
        treeFile.write((char *) &t, sizeof(BTNode));
        write++;

        if(recAddr == rootAddr)
            root = t;
    }
    //Splitting time
    else
    {
        cout<<"Split Occurring"<<endl;
        splitNode (key, recAddr, oneAddr,twoAddr);

    }  
    
}

BTNode BTree::getNode (int recAddr)
{
    BTNode t;
    //Jump to address of node,read it in, increment total read, and return it
    treeFile.seekg(recAddr,ios::beg);
    treeFile.read((char *) &t, sizeof(BTNode));
    read++;
    return t;
}

void BTree::printTree(int recAddr)
{
    //Check if the current node is a leaf
    if(recAddr != -1)
    {
        BTNode dummy = getNode(recAddr);
        printNode(recAddr);
        for(int i=0;i<=dummy.currSize;i++)
        {
            printTree(dummy.child[i]);
        }
    }

}

void BTree::placeNode (keyType key, int recAddr, int oneAddr, int twoAddr)
{
    if(recAddr == -1)
    {
        adjRoot(key, oneAddr,twoAddr);
        return;
    }
    BTNode t = getNode(recAddr);
    //Node not full
    if(t.currSize < ORDER -1)
    {
        int i;
        //Slide contents to the left
        for(i =t.currSize -1; i>=0; i--)
        {
            if(key < t.contents[i])
            {
                t.contents[i+1] = t.contents[i];
                t.child[i+2] = t.child[i+1];
            }
            else
                break;
        }
        //Insert key into current position
        t.contents[i+1] = key;
        t.child[i+2] = twoAddr;
        t.currSize++;

        treeFile.seekg(recAddr,ios::beg);
        treeFile.write((char *) &t, sizeof(BTNode));
        write++;

        if(recAddr == rootAddr)
            root = t;
    }
    //Splitting time
    else
    {
        cout<<"Split Occurring Again"<<endl;
        splitNode (key, recAddr, oneAddr,twoAddr);

    }
}

bool BTree::isLeaf (int recAddr)
{
    BTNode t = getNode(recAddr);
    for(int i=0;i<=t.currSize;i++)
    {
        if(t.child[i] != -1)
        {
            return false;
        }
    }
    return true;

}

bool BTree::isLeaf(BTNode t)
{
    for(int i=0;i<=t.currSize;i++)
    {
        if(t.child[i] != -1)
        {
            return false;
        }
    }
    return true;
}	

int BTree::countLeaves(int recAddr)
{
    BTNode t = getNode(recAddr);
    bool isLeaf = true;
    int sum =0;
    for(int i=0;i<=t.currSize;i++)
    {
        if(t.child[i] != -1)
        {
            isLeaf = true;
            sum += countLeaves(t.child[i]);
        }
    }
    return isLeaf ? 1 : sum;
}

//Adjust root
void BTree::adjRoot (keyType rootElem, int oneAddr, int twoAddr)
{
    BTNode t;
    t.child[0] = oneAddr;
    t.child[1] = twoAddr;
    t.contents[0] = rootElem;
    t.currSize = 1;
    root = t;
    treeFile.seekg(0,ios::end);
    rootAddr = treeFile.tellg();
    treeFile.write((char*) &t, sizeof(BTNode));
    write++;
    treeFile.seekg(0,ios::beg);
    BTNode header;
    header.child[0] = rootAddr;
    treeFile.write((char *) &header, sizeof(BTNode));
    write++;
    height++;

}

void BTree::splitNode (keyType& key,int recAddr,int& oneAddr,int& twoAddr)
{
        BTNode t = getNode(recAddr);
        BTNode rightNode;
        keyType temp;
        int tempAddr;

        //Check if key is the largest value compared to the other values in the node
        if(key < t.contents[t.currSize-1])
        {
            temp = t.contents[t.currSize-1];
            t.contents[t.currSize-1] = key;
            key = temp;

            tempAddr = twoAddr;
            twoAddr = t.child[t.currSize];
            t.child[t.currSize] = tempAddr;

            //Sort Node now that we added key
            for(int i= t.currSize-1; i> 0;i--)
            {
                if(t.contents[i] < t.contents[i-1])
                {
                    temp = t.contents[i-1];
                    t.contents[i-1] = t.contents[i];
                    t.contents[i] = temp;

                    tempAddr = t.child[i];
                    t.child[i] = t.child[i+1];
                    t.child[i+1] = tempAddr;
                }
                else
                    break;
            }
        }

        //Left node now is just size 2 with index 0 and 1
        int middle = ORDER/2;
        t.currSize = middle;

        rightNode.currSize = ORDER - 1 - middle;
        rightNode.contents[0] = t.contents[3];
        rightNode.contents[1] = key;
        rightNode.child[0] = t.child[3];
        rightNode.child[1] = t.child[4];
        rightNode.child[2] = twoAddr;
        rightNode.child[3] = -1;
        rightNode.child[4] = -1;

        treeFile.seekg(recAddr,ios::beg);
        treeFile.write((char *) &t, sizeof(BTNode));

        treeFile.seekg(0,ios::end);
        int rightNodeAddress = treeFile.tellg();
        treeFile.write((char *) &rightNode, sizeof(BTNode));
        placeNode(t.contents[middle], findpAddr(key,root, rootAddr, recAddr), recAddr ,rightNodeAddress);
}

bool BTree::search (string key, BTNode t, int tAddr)
{
    for(int i=0; i< t.currSize;i++)
    {
        //Compare key to album UPC
        if(strcmp(key.c_str(), t.contents[i].getUPC().c_str()) ==  0)
        {
            return true;
        }
    }

    for(int i=0; i < t.currSize; i++)
    {
        //If key is smaller, then check all UPCs before this value
        if(key < t.contents[i].getUPC())
        {
            if(t.child[i] == -1)
                return false;
            else
                return search(key,getNode(t.child[i]), t.child[i]);
        }
    }

    if(t.child[t.currSize] == -1)
    {
        return false;
    }
    else
    {
        //Search branch all the way at the end
        return search(key,getNode(t.child[t.currSize]), t.child[t.currSize]);
    }
    
}