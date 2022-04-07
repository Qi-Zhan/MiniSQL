#include"head.h"
#include"BufferManager.h"
/*第0块除了col name 初始化还要下一个可用的块号,说明采用样例报告中的存储结构*/

//delete all indexManager直接createIndex
//select all 不需要调用indexManager
class BPlusTree{ //与bufferManager对接
public:
    int insertKey(std::string key, record pointer);
    int deleteKey(mycondition condition, std::vector<record>& recordList,int root); //要返回vector record 可能需要一个引用来存储
    void initialize(std::string indexName,std::string colName,int type){
        this->indexName = indexName;
        // this->colName = colName;
        this->type = type;
        this->size = (type <= 0) ? 4 : type;
        this->Branch = ((type <= 0 )? (BLOCK_SIZE - 10) / (8 + 4) : (BLOCK_SIZE - 10) / (8 + type)-2);
    };
    void test(int index_id);//调试函数
    int insertkey(std::string key, record pointer);
    record findKey(std::string key, int root); //-1表示没找到 返回块号和一串list
private:
    int size; //int 4 float 4 char n
    int type; //int 0 float -1 char n
    int Branch;//几个pointer
    std::string indexName;
    // std::string colName;//不知道需不需要
    std::vector<int> uselist; //记录用到的块号,flush用
    
    int getFreeBlock();//顺便指向下一个Block ///
    int split(int block_id); //返回的是另一个分裂出的结点的block_id
    int splitRoot(int block_id); //返回的是另一个分裂出的结点的block_id
    // int combine();
    // void updateKey(int Node);
    int insertLeaf(int block_id, std::string key, record onerecord);
    int compare(std::string a, std::string b); //考虑type的compare
    void copyto(std::string key, char a[]);//考虑type的copy
    void moveKey(int dst, int src, int begin, int end, int start, int offset, int baseDst, int baseSrc); //des指buffer里的id begin指第几个而不是直接地址
    void insertoneLeaf(int block_id, std::string key, record onerecord, int base);
    inline bool isLeaf(int block_id) { return (buffer.Page[block_id].pool[0] == 'L') ? 1 : 0; };
    inline bool isInternal(int block_id) { return (buffer.Page[block_id].pool[0] == 'I') ? 1 : 0; };
    inline bool leafFull(int block_id){
        int nowBranch = char4int(buffer.Page[block_id].pool + 1);
        return (Branch == nowBranch) ? 1 : 0; //full 即表示需要分裂
    };
    inline int keyNumber(int block_id) { return char4int(buffer.Page[block_id].pool + 1); };
    inline bool internalFull(int block_id){
        int nowBranch = char4int(buffer.Page[block_id].pool + 1);
        return (Branch == nowBranch + 1) ? 1 : 0;
    };
};
