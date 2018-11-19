#include <lmdb.h>
#include <iostream>
//#include <direct.h>
#include <string>
using namespace std;

inline void MDB_CHECK(int mdb_status){//错误检测
    if (mdb_status != MDB_SUCCESS){
        cout << "error" <<mdb_strerror(mdb_status) << endl;
        return;
    }
    else{
        return;
    }
}

int main(int argc, char* argv[])
{
    string key("hello man");
    string value("20160713");
    const size_t LMDB_MAP_SIZE = 1073741824;//lmdb的大小
    MDB_env* mdb_env_;
    MDB_dbi mdb_dbi_;

    MDB_CHECK(mdb_env_create(&mdb_env_));
    MDB_CHECK(mdb_env_set_mapsize(mdb_env_, LMDB_MAP_SIZE));

#if 0
    if (_mkdir("C:\\Users\\Dominator\\Desktop\\ssd\\temp") != 0){
        cout << "mkdir failed\n";
        //system("pause");
        //return;
    }
#endif
    
    int flag = 0;
    MDB_CHECK(mdb_env_open(mdb_env_, "./lmdb_db", flag, 0664));
    cout << "open lmdb " << "./lmdb_db" << endl;
    //write
    MDB_txn* mdb_txn;
    MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, 0, &mdb_txn));
    MDB_CHECK(mdb_dbi_open(mdb_txn, NULL, 0, &mdb_dbi_));
    MDB_val mdb_key, mdb_value;
    mdb_key.mv_data = const_cast<char*>(key.data());
    mdb_key.mv_size = key.size();
    mdb_value.mv_data = const_cast<char*>(value.data());
    mdb_value.mv_size = value.size();
    MDB_CHECK(mdb_put(mdb_txn, mdb_dbi_, &mdb_key, &mdb_value, 0));
    MDB_CHECK(mdb_txn_commit(mdb_txn));

    if (mdb_env_ != NULL){
        mdb_dbi_close(mdb_env_, mdb_dbi_);
        mdb_env_close(mdb_env_);
        mdb_env_ = NULL;
    }

    flag = MDB_RDONLY | MDB_NOTLS;
    MDB_CHECK(mdb_env_create(&mdb_env_));
    MDB_CHECK(mdb_env_set_mapsize(mdb_env_, LMDB_MAP_SIZE));
    MDB_CHECK(mdb_env_open(mdb_env_, "./lmdb_db", flag, 0664));
    cout << "open lmdb " << "./lmdb_db" << endl;
    MDB_cursor* mdb_cursor;
    MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, MDB_RDONLY, &mdb_txn));
    MDB_CHECK(mdb_dbi_open(mdb_txn, NULL, 0, &mdb_dbi_));
    MDB_CHECK(mdb_cursor_open(mdb_txn, mdb_dbi_, &mdb_cursor));

    mdb_cursor_get(mdb_cursor, &mdb_key, &mdb_value, MDB_FIRST);
    cout << string(static_cast<const char*>(mdb_key.mv_data), mdb_key.mv_size) << endl;
    cout << string(static_cast<const char*>(mdb_value.mv_data), mdb_value.mv_size) << endl;

    //system("pause");
    return 0;
}
