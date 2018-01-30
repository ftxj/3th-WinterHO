#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
using namespace std;

int NumberOfProcess = 0;
int Quantum = 0;

enum Statement{
  Assisgn, Output, Lock, UnLock, End
};

enum State{
  Run, Ready, Blocked, Ended
};

struct PCB;

int Time[5] = {0};

bool Locked = false;

int Variable[27] = {0};

int BlockQueue[500];
int blockTop = -1;
int blockTail = 0;
int blockEmpty = 0;


int ReadyQueue[500];
int readyTop = -1;
int readyTail = 0;
int readyEmpty = 0;

inline void insertBlock(int PID);
inline void insertReady(int PID);
inline void insertReadyFront(int PID);
inline int popReady();
inline int popBlock();

struct PCB{
public:  
  struct code{
    int id;
    int var;
    int value;
  };
  code Codes[30];

  int PID = 0;
  int State = 0;
  int Quantum = 0;
  int sentencesNumber = 0;
  int PC = 0;
  
  void getOneStatement(int id, int var = 0, int value = 0){
    Codes[sentencesNumber].id = id;
    Codes[sentencesNumber].var = var;
    Codes[sentencesNumber].value = value;
    sentencesNumber++;
  }
  
  void doNextSentence(){
    int id = Codes[PC].id;
    int var = Codes[PC].var;
    int value = Codes[PC].value;
    switch(id){
      case Assisgn: 
        Variable[var] = value; 
        break;
      case Output: 
        printf("%d: %d\n", PID + 1, Variable[var]); 
        break;
      case Lock:  
        if(Locked == false) Locked = true;
        else insertBlock(PID);
        break;
      case UnLock: 
        Locked = false;
        if(blockEmpty != 0){
          int id = popBlock();
          insertReadyFront(id);
        }
        break;
      case End: 
        State = Ended; 
        break;
      default: break;
    }
    if(State == Run) PC++;
    this->Quantum =this-> Quantum - Time[id];
    if(this->Quantum <= 0 && State == Run){
      insertReady(PID);
    }
  }
};

PCB PcbHead[500];

inline void insertBlock(int PID){
  PcbHead[PID].State = Blocked;
  blockEmpty++;
  blockTop = (++blockTop) % NumberOfProcess;  
  BlockQueue[blockTop] = PID;
}

inline void insertReady(int PID){
  PcbHead[PID].State = Ready;
  readyEmpty++;
  readyTop = (++readyTop) % NumberOfProcess;  
  ReadyQueue[readyTop] = PID;
}

inline void insertReadyFront(int PID){
  PcbHead[PID].State = Ready;
  readyEmpty++;
  readyTail = (NumberOfProcess + (--readyTail)) % NumberOfProcess;
  ReadyQueue[readyTail] = PID;
}

inline int popReady(){
  int res = ReadyQueue[readyTail];
  readyEmpty--;
  readyTail = (++readyTail) % NumberOfProcess;  
  return res;
}

inline int popBlock(){
  int res = BlockQueue[blockTail];
  blockEmpty--;
  blockTail = (++blockTail) % NumberOfProcess;
  return res;
}


void RunOS(){
  while(readyEmpty != 0){
    int RunID = popReady();
    PcbHead[RunID].Quantum = Quantum;
    PcbHead[RunID].State = Run;
    while(PcbHead[RunID].State == Run){
      PcbHead[RunID].doNextSentence();
    }
  }
}

void init(){
  cin >> NumberOfProcess;
  cin >> Time[0] >> Time[1] >> Time[2] >> Time[3] >> Time[4];
  cin >> Quantum;

  Locked = false;

  blockTop = -1;
  blockTail = 0;
  blockEmpty = 0;

  readyTop = -1;
  readyTail = 0;
  readyEmpty = 0;
  
  memset(Variable, 0, 27 * sizeof(int));
  memset(BlockQueue, 0, 500 * sizeof(int));
  memset(ReadyQueue, 0, 500 * sizeof(int));

  for(int i = 0; i <= NumberOfProcess; ++i){
    PcbHead[i].sentencesNumber = 0;
    PcbHead[i].PID = i;
    PcbHead[i].State = 0;
    PcbHead[i].Quantum = 0;
    PcbHead[i].PC = 0;
    memset(PcbHead[i].Codes, 0, 30 * sizeof(PCB::code));
  }

  for(int i = 0; i < NumberOfProcess; ++i){
    insertReady(i);
  }
}

int main(){
  int N;
  string code;  
  cin >> N;
  int k = 1;
  while(N--){
    init();
    for(int i = 0; i < NumberOfProcess; ){
      getline(cin, code);
      char v;    
      string s;
      int id = -1, value;
      stringstream codeStream(code);

      if(code == "end") id = End;
      else if(code == "lock") id = Lock;
      else if(code == "unlock") id = UnLock;
      else if(code.find("=") != string::npos) id = Assisgn;
      else if(code.find("print") != string::npos)  id = Output;

      if(id == Assisgn) codeStream >> v >> s >> value; 
      if(id == Output) codeStream >> s >> v;
      if(id != -1) PcbHead[i].getOneStatement(id, int(v - 'a'), value);
      
      if(id == End) ++i;
    }
    RunOS();
    if(N != 0) cout << endl;
  }
  return 0;
}