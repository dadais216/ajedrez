
#if debugMode
void debugShowAndWait(const char*,bool);
void debugShowAndWaitMem(const char*,bool);

RectangleShape backgroundMem;
RectangleShape backgroundMemDebug;
RectangleShape localMemorySeparator;
Text textValMem;

RectangleShape posPiece;
RectangleShape posActGood;
RectangleShape posActBad;
RectangleShape posMem;
RectangleShape* tileActDebug;

Text textDebug;

//necesito diferir los dibujados de debug para que se hagan despues de que se dibujen las demas cosas.
//tengo un vector de ints donde estan las instrucciones y los datos de este frame. Esta bueno porque tengo todo
//el codigo en un lugar solo
vector<int> debugDrawChannel;
enum{
     tdebugSetIndirectColor=-9999,tdebugUnsetIndirectColor,
     tdebugDrawCte,tdebugDrawPosX,tdebugDrawPosY,
     tdebugDrawPos, 
};


void debugUpdateAndDrawBuckets();
void debugUpdateAndDrawBucketsInit(bool);

void drawBucketdesoptHNodes(desoptHolder*,desoptHolder::node*,sf::Color);

#endif

