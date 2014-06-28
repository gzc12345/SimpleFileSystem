/*
* 模拟操作系统文件系统
* 葛志诚 2014-06-25
* 使用数组模拟硬盘，存储所有文件的inode信息
* 使用数组模拟硬盘，存储所有目录信息
* 目录只存放文件名，类别，和数组下标
* 需要编写生成目录的算法，生成文件的算法，以及对应的删除算法
* 需要写类似于shell命令ls cd mkdir rm 的算法
*/
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<sstream>

using namespace std;

typedef struct inode{
	int size;			//文件字符计数
	string atime;		//访问时间
	string mtime;		//修改时间
	string data;		//文件内容
}INODE,*INODEPTR;

typedef struct dir {
	string d_name;		//目录名
	int d_size;			//子目录计数
	int f_size;			//包含文件计数
	int total_size;		//总字节计数
	map<string,int> file_map;	//包含文件指针
	map<string,int> dir_map;	//子目录指针
	int parent;			//指向父目录的指针
}DIR,*DIRPTR;

vector<pair<INODE,int>> INODELIST;	//存放所有文件的INODE，int为标记，为0表示已删除,通过下标索引
vector<pair<DIR,int>> DIRLIST;	//存放所有目录的DIR，int为标记，为0表示已删除，通过下标索引

/*
* 增加文件，参数cu_dir为当前目录指针，fileName文件名，inode文件属性
*/
int addFile(int cu_dir,string fileName,INODE inode) {
	//需要在INODELIST中增加一个INODE
	//需要在目录中增加文件指针
	//注意修改目录属性
	if(cu_dir > DIRLIST.size())
		return -1;
	if(DIRLIST.at(cu_dir).second == 0)
		return -1;
	DIR pDir = DIRLIST.at(cu_dir).first;	//父目录
	if(pDir.file_map.count(fileName) > 0)
		return -1;
	unsigned int ix;//找到第一个tag为0的目录项，否则返回新目录项
	for(ix = 0;ix < INODELIST.size();ix++)
		if(INODELIST.at(ix).second == 0)
			break;
	pDir.file_map.insert(make_pair(fileName,ix));//插入父目录表项
	pDir.f_size ++;//更新计数
	pDir.total_size ++;//更新计数
	DIRLIST[cu_dir].first = pDir;//更新外存 
	if(ix < INODELIST.size()) 
		INODELIST[ix] = make_pair(inode,1);
	else
		INODELIST.push_back(make_pair(inode,1));
	return ix;
}

/*
* 删除文件，参数cu_dir为当前目录指针，fileName文件名
*/
int deleteFile(int cu_dir,string fileName) {
	//通过当前目录指针找到当目录
	//找到相应的文件指针
	//删除目录项和文件，修改目录属性
	if(cu_dir > DIRLIST.size())
		return -1;
	if(DIRLIST.at(cu_dir).second == 0)
		return -1;
	DIR pDir = DIRLIST.at(cu_dir).first;	//父目录
	if(pDir.file_map.count(fileName) == 0)
		return -1;
	int filePtr = pDir.file_map[fileName];
	//更新父目录
	pDir.file_map.erase(fileName);
	pDir.f_size --;
	pDir.total_size --;
	DIRLIST[cu_dir].first = pDir;
	//删除文件
	INODELIST[filePtr].second = 0;
	return 1;
}

/*
* 增加目录，参数cu_dir为当前目录指针，dirName目录名,返回目录数组下标
*/
int addDir(int cu_dir,string dirName) {
	//需要在目录中增加目录指针
	//需要在在DIRLIST中创建目录节点
	//注意修改目录属性
	if(DIRLIST.size() == 0) {
		//创建根目录
		DIR dir;
		dir.d_name = dirName;
		dir.d_size = dir.f_size = dir.total_size = 0;
		dir.parent = 0;
		DIRLIST.push_back(make_pair(dir,1));
		return 0;//根目录下标为0;
	}
	if(cu_dir > DIRLIST.size())
		return -1;
	if(DIRLIST.at(cu_dir).second == 0)
		return -1;
	DIR pDir = DIRLIST.at(cu_dir).first;	//父目录
	if(pDir.dir_map.count(dirName) > 0)
		return -1;
	//新建DIRLIST项
	unsigned int ix;//找到第一个tag为0的目录项，否则返回新目录项
	for(ix = 0;ix < DIRLIST.size();ix++)
		if(DIRLIST.at(ix).second == 0)
			break;
	pDir.dir_map.insert(make_pair(dirName,ix));//插入父目录表项
	pDir.d_size ++ ;//更新计数 
	pDir.total_size ++;//更新计数
	DIRLIST[cu_dir].first = pDir;//更新外存 
	DIR sDir;	//子目录
	sDir.d_name = dirName;
	sDir.d_size = sDir.f_size = sDir.total_size = 0;
	sDir.parent = cu_dir;
	if(ix < DIRLIST.size())
		DIRLIST[ix] = make_pair(sDir,1);
	else
		DIRLIST.push_back(make_pair(sDir,1));
	return ix;
}

/*
* 递归删除目录，参数cu_dir为当前目录指针，sub_dir为子目录指针
*/
int deleteDirR(int dirPtr) {
	//需要递归遍历目录
	//需要在在DIRLIST中删除目录节点
	//注意修改目录属性
	DIR dir = DIRLIST[dirPtr].first;
	for(map<string,int>::iterator iter = dir.file_map.begin();iter != dir.file_map.end();iter++) {
		string fileName = iter->first;
		deleteFile(dirPtr,fileName);
	}
	dir = DIRLIST[dirPtr].first;
	vector<string> subNameList;
	for(map<string,int>::iterator iter = dir.dir_map.begin();iter != dir.dir_map.end();iter++) {
		string subName = iter->first;
		int subPtr = iter->second;
		deleteDirR(subPtr);
		subNameList.push_back(subName);
	}
	for(unsigned int ix = 0;ix < subNameList.size();ix ++)
		dir.dir_map.erase(subNameList[ix]);
	DIRLIST[dirPtr].first = dir;
	DIRLIST[dirPtr].second = 0;
	return 1;
}
/*
* 删除目录，参数cu_dir为当前目录指针，sub_dir为子目录指针
*/
int deleteDir(int cu_dir,string sub_dir) {
	//需要递归遍历目录
	//需要在在DIRLIST中删除目录节点
	//注意修改目录属性
	if(cu_dir > DIRLIST.size())
		return -1;
	if(DIRLIST.at(cu_dir).second == 0)
		return -1;
	DIR pDir = DIRLIST.at(cu_dir).first;	//父目录
	if(pDir.dir_map.count(sub_dir) == 0)
		return -1;
	int dirPtr = pDir.dir_map[sub_dir];
	deleteDirR(dirPtr);
	pDir.dir_map.erase(sub_dir);
	pDir.d_size --;
	pDir.total_size --;
	DIRLIST[cu_dir].first = pDir;
	return 1;
}

//按存储顺序输出所有目录，ctl不为fause输出详细信息
void printDirList(bool ctl) {
	cout<<"DIRLIST:"<<endl;
	for(unsigned int ix = 0;ix != DIRLIST.size();ix++) {
		DIR dir = DIRLIST[ix].first;
		int tag = DIRLIST[ix].second;
		cout<<ix<<"\tdirname:"<<dir.d_name<<" existed:"<<tag<<endl;
		if(ctl) {
			cout<<"\t子目录:"<<dir.d_size<<endl;
			for(map<string,int>::iterator itd = dir.dir_map.begin();itd != dir.dir_map.end();itd ++) 
				cout<<"\tsubdir:"<<itd->first<<" ptr:"<<itd->second<<endl;
			cout<<"\t文件:"<<dir.f_size<<endl;
			for(map<string,int>::iterator itf = dir.file_map.begin();itf != dir.file_map.end();itf ++) 
				cout<<"\tfilename:"<<itf->first<<" ptr:"<<itf->second<<endl;
			cout<<"\ttotalsize:"<<dir.total_size<<endl;
		}
	}
	cout<<endl;
}

//按存储顺序输出所有文件inode,ctl不为fause输出相信信息
void printFileList(bool ctl) {
	cout<<"INODELIST:"<<endl;
	for(unsigned int ix = 0;ix != INODELIST.size();ix++) {
		INODE inode = INODELIST[ix].first;
		int tag = INODELIST[ix].second;
		cout<<ix<<"\tsize:"<<inode.size<<" existed:"<<tag<<endl;
		if(ctl) {
			cout<<"\tatime:"<<inode.atime<<endl;
			cout<<"\tmtime:"<<inode.mtime<<endl;
			cout<<"\tcontent:"<<inode.data<<endl;
		}
	}
}

void printDirTree(int cu_dir = 0,int level = 0) {
	if(DIRLIST.size() == 0 || DIRLIST[cu_dir].second == 0)
		return;
	DIR dir = DIRLIST[cu_dir].first;
	for(map<string,int>::iterator iter = dir.file_map.begin();iter != dir.file_map.end();iter ++) {
		for(int i = 0;i < level;i++) cout<<"-->";
		cout<<"F:"<<iter->first<<endl;
	}
	for(map<string,int>::iterator iter = dir.dir_map.begin();iter != dir.dir_map.end();iter ++) {
		for(int i = 0;i < level;i++) cout<<"-->";
		cout<<"D:"<<iter->first<<endl;
		printDirTree(iter->second,level + 1);
	}

}

/*
*ls
*/
int ls(int dirPtr) {
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//父目录
	for(map<string,int>::iterator iter = dir.file_map.begin();iter != dir.file_map.end();iter ++)
		cout<<"F:"<<iter->first<<"\tsize:"<<INODELIST.at(iter->second).first.size<<endl;
	for(map<string,int>::iterator iter = dir.dir_map.begin();iter != dir.dir_map.end();iter ++)
		cout<<"D:"<<iter->first<<"\tsize:"<<DIRLIST.at(iter->second).first.total_size<<endl;
	return 1;
}
/*
*cd 返回进入的目录指针，否则返回-1
*/
int cd(int dirPtr,string dirName) {
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//父目录
	if(dirName == "..") return dir.parent;
	if(dir.dir_map.count(dirName) == 0) return -1;
	return dir.dir_map[dirName];
}
/*
*rm
*/
int rm(int dirPtr,string name) {
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//父目录
	int choice = dir.file_map.count(name) + dir.dir_map.count(name);
	switch(choice) {
	case 1:
		if(dir.file_map.count(name))
			return deleteFile(dirPtr,name);
		if(dir.dir_map.count(name))
			return deleteDir(dirPtr,name);
		break;
	case 2:
		return deleteFile(dirPtr,name) + deleteDir(dirPtr,name);
		break;
	default:break;
	}
	return -1;
}
/*
* mkfile AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
*/
int mkfile(int dirPtr,string name) {
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//父目录
	//GO ON
}
/*
*mkdir BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
*/
int mkdir(int dirPtr,string dirname) {
	INODE inode;//需要构造新文件INODE节点属性
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//父目录
	//GO ON
}
/*
*cat 查看文件各种属性 CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
*/
int cat(int dirPtr,string filename) {
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//父目录
	//GO ON
}
/*
*chfile DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
*/
int chfile(int dirPtr,string filename) {
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//父目录
	//GO ON
}
void shell() {
	int CUPTR = 0;
	cout<<DIRLIST[CUPTR].first.d_name<<"$ ";
	string cmd;
	while(getline(cin,cmd)) {
		stringstream cmdstream(cmd);
		string cmdName;
		cmdstream>>cmdName;
		vector<string> paraList;
		string para;
		while(cmdstream>>para) paraList.push_back(para);
		if(cmdName == "ls") {
			if(paraList.size() == 0) ls(CUPTR);
			else cerr<<"多余参数"<<endl;

		}
		else if(cmdName == "cd") {
			if(paraList.size() == 0) CUPTR = 0;
			else if(paraList.size() == 1) CUPTR = cd(CUPTR,paraList[0]);
			else cerr<<"多余参数"<<endl;
		}
		else if(cmdName == "rm") {
			for(unsigned int ix = 0; ix < paraList.size();ix ++)
				if(rm(CUPTR,paraList[ix]) < 0)
					cerr<<"No such file or dir"<<endl;
		}
		cout<<DIRLIST[CUPTR].first.d_name<<"$ ";
	}
}

int main() {
	addDir(0,"/");
	addDir(0,"a");
	addDir(0,"b");
	addDir(0,"c");
	addDir(3,"d");
	INODE inode;
	inode.data = "dasdasdas";
	inode.size = inode.data.length();
	inode.atime = inode.mtime = "--";
	addFile(1,"file1",inode);
	addFile(1,"file2",inode);
	addFile(1,"file3",inode);
	addFile(2,"file1",inode);
	addFile(2,"file2",inode);
	addFile(2,"file3",inode);
	addFile(3,"file1",inode);
	addFile(3,"file2",inode);
	addFile(3,"file3",inode);
	addFile(4,"file1",inode);
	addFile(4,"file2",inode);
	addFile(4,"file3",inode);
	printDirList(true);
	printFileList(true);
	printDirTree();
	shell();
	//deleteDir(0,"c");
	//printDirList(true);
	//printFileList(true);
	//printDirTree();
	system("pause");
	return 0;
}