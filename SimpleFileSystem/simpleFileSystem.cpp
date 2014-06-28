/*
* ģ�����ϵͳ�ļ�ϵͳ
* ��־�� 2014-06-25
* ʹ������ģ��Ӳ�̣��洢�����ļ���inode��Ϣ
* ʹ������ģ��Ӳ�̣��洢����Ŀ¼��Ϣ
* Ŀ¼ֻ����ļ�������𣬺������±�
* ��Ҫ��д����Ŀ¼���㷨�������ļ����㷨���Լ���Ӧ��ɾ���㷨
* ��Ҫд������shell����ls cd mkdir rm ���㷨
*/
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<sstream>

using namespace std;

typedef struct inode{
	int size;			//�ļ��ַ�����
	string atime;		//����ʱ��
	string mtime;		//�޸�ʱ��
	string data;		//�ļ�����
}INODE,*INODEPTR;

typedef struct dir {
	string d_name;		//Ŀ¼��
	int d_size;			//��Ŀ¼����
	int f_size;			//�����ļ�����
	int total_size;		//���ֽڼ���
	map<string,int> file_map;	//�����ļ�ָ��
	map<string,int> dir_map;	//��Ŀ¼ָ��
	int parent;			//ָ��Ŀ¼��ָ��
}DIR,*DIRPTR;

vector<pair<INODE,int>> INODELIST;	//��������ļ���INODE��intΪ��ǣ�Ϊ0��ʾ��ɾ��,ͨ���±�����
vector<pair<DIR,int>> DIRLIST;	//�������Ŀ¼��DIR��intΪ��ǣ�Ϊ0��ʾ��ɾ����ͨ���±�����

/*
* �����ļ�������cu_dirΪ��ǰĿ¼ָ�룬fileName�ļ�����inode�ļ�����
*/
int addFile(int cu_dir,string fileName,INODE inode) {
	//��Ҫ��INODELIST������һ��INODE
	//��Ҫ��Ŀ¼�������ļ�ָ��
	//ע���޸�Ŀ¼����
	if(cu_dir > DIRLIST.size())
		return -1;
	if(DIRLIST.at(cu_dir).second == 0)
		return -1;
	DIR pDir = DIRLIST.at(cu_dir).first;	//��Ŀ¼
	if(pDir.file_map.count(fileName) > 0)
		return -1;
	unsigned int ix;//�ҵ���һ��tagΪ0��Ŀ¼����򷵻���Ŀ¼��
	for(ix = 0;ix < INODELIST.size();ix++)
		if(INODELIST.at(ix).second == 0)
			break;
	pDir.file_map.insert(make_pair(fileName,ix));//���븸Ŀ¼����
	pDir.f_size ++;//���¼���
	pDir.total_size ++;//���¼���
	DIRLIST[cu_dir].first = pDir;//������� 
	if(ix < INODELIST.size()) 
		INODELIST[ix] = make_pair(inode,1);
	else
		INODELIST.push_back(make_pair(inode,1));
	return ix;
}

/*
* ɾ���ļ�������cu_dirΪ��ǰĿ¼ָ�룬fileName�ļ���
*/
int deleteFile(int cu_dir,string fileName) {
	//ͨ����ǰĿ¼ָ���ҵ���Ŀ¼
	//�ҵ���Ӧ���ļ�ָ��
	//ɾ��Ŀ¼����ļ����޸�Ŀ¼����
	if(cu_dir > DIRLIST.size())
		return -1;
	if(DIRLIST.at(cu_dir).second == 0)
		return -1;
	DIR pDir = DIRLIST.at(cu_dir).first;	//��Ŀ¼
	if(pDir.file_map.count(fileName) == 0)
		return -1;
	int filePtr = pDir.file_map[fileName];
	//���¸�Ŀ¼
	pDir.file_map.erase(fileName);
	pDir.f_size --;
	pDir.total_size --;
	DIRLIST[cu_dir].first = pDir;
	//ɾ���ļ�
	INODELIST[filePtr].second = 0;
	return 1;
}

/*
* ����Ŀ¼������cu_dirΪ��ǰĿ¼ָ�룬dirNameĿ¼��,����Ŀ¼�����±�
*/
int addDir(int cu_dir,string dirName) {
	//��Ҫ��Ŀ¼������Ŀ¼ָ��
	//��Ҫ����DIRLIST�д���Ŀ¼�ڵ�
	//ע���޸�Ŀ¼����
	if(DIRLIST.size() == 0) {
		//������Ŀ¼
		DIR dir;
		dir.d_name = dirName;
		dir.d_size = dir.f_size = dir.total_size = 0;
		dir.parent = 0;
		DIRLIST.push_back(make_pair(dir,1));
		return 0;//��Ŀ¼�±�Ϊ0;
	}
	if(cu_dir > DIRLIST.size())
		return -1;
	if(DIRLIST.at(cu_dir).second == 0)
		return -1;
	DIR pDir = DIRLIST.at(cu_dir).first;	//��Ŀ¼
	if(pDir.dir_map.count(dirName) > 0)
		return -1;
	//�½�DIRLIST��
	unsigned int ix;//�ҵ���һ��tagΪ0��Ŀ¼����򷵻���Ŀ¼��
	for(ix = 0;ix < DIRLIST.size();ix++)
		if(DIRLIST.at(ix).second == 0)
			break;
	pDir.dir_map.insert(make_pair(dirName,ix));//���븸Ŀ¼����
	pDir.d_size ++ ;//���¼��� 
	pDir.total_size ++;//���¼���
	DIRLIST[cu_dir].first = pDir;//������� 
	DIR sDir;	//��Ŀ¼
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
* �ݹ�ɾ��Ŀ¼������cu_dirΪ��ǰĿ¼ָ�룬sub_dirΪ��Ŀ¼ָ��
*/
int deleteDirR(int dirPtr) {
	//��Ҫ�ݹ����Ŀ¼
	//��Ҫ����DIRLIST��ɾ��Ŀ¼�ڵ�
	//ע���޸�Ŀ¼����
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
* ɾ��Ŀ¼������cu_dirΪ��ǰĿ¼ָ�룬sub_dirΪ��Ŀ¼ָ��
*/
int deleteDir(int cu_dir,string sub_dir) {
	//��Ҫ�ݹ����Ŀ¼
	//��Ҫ����DIRLIST��ɾ��Ŀ¼�ڵ�
	//ע���޸�Ŀ¼����
	if(cu_dir > DIRLIST.size())
		return -1;
	if(DIRLIST.at(cu_dir).second == 0)
		return -1;
	DIR pDir = DIRLIST.at(cu_dir).first;	//��Ŀ¼
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

//���洢˳���������Ŀ¼��ctl��Ϊfause�����ϸ��Ϣ
void printDirList(bool ctl) {
	cout<<"DIRLIST:"<<endl;
	for(unsigned int ix = 0;ix != DIRLIST.size();ix++) {
		DIR dir = DIRLIST[ix].first;
		int tag = DIRLIST[ix].second;
		cout<<ix<<"\tdirname:"<<dir.d_name<<" existed:"<<tag<<endl;
		if(ctl) {
			cout<<"\t��Ŀ¼:"<<dir.d_size<<endl;
			for(map<string,int>::iterator itd = dir.dir_map.begin();itd != dir.dir_map.end();itd ++) 
				cout<<"\tsubdir:"<<itd->first<<" ptr:"<<itd->second<<endl;
			cout<<"\t�ļ�:"<<dir.f_size<<endl;
			for(map<string,int>::iterator itf = dir.file_map.begin();itf != dir.file_map.end();itf ++) 
				cout<<"\tfilename:"<<itf->first<<" ptr:"<<itf->second<<endl;
			cout<<"\ttotalsize:"<<dir.total_size<<endl;
		}
	}
	cout<<endl;
}

//���洢˳����������ļ�inode,ctl��Ϊfause���������Ϣ
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
	DIR dir = DIRLIST.at(dirPtr).first;	//��Ŀ¼
	for(map<string,int>::iterator iter = dir.file_map.begin();iter != dir.file_map.end();iter ++)
		cout<<"F:"<<iter->first<<"\tsize:"<<INODELIST.at(iter->second).first.size<<endl;
	for(map<string,int>::iterator iter = dir.dir_map.begin();iter != dir.dir_map.end();iter ++)
		cout<<"D:"<<iter->first<<"\tsize:"<<DIRLIST.at(iter->second).first.total_size<<endl;
	return 1;
}
/*
*cd ���ؽ����Ŀ¼ָ�룬���򷵻�-1
*/
int cd(int dirPtr,string dirName) {
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//��Ŀ¼
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
	DIR dir = DIRLIST.at(dirPtr).first;	//��Ŀ¼
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
	DIR dir = DIRLIST.at(dirPtr).first;	//��Ŀ¼
	//GO ON
}
/*
*mkdir BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
*/
int mkdir(int dirPtr,string dirname) {
	INODE inode;//��Ҫ�������ļ�INODE�ڵ�����
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//��Ŀ¼
	//GO ON
}
/*
*cat �鿴�ļ��������� CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
*/
int cat(int dirPtr,string filename) {
	if(dirPtr > DIRLIST.size())
		return -1;
	if(DIRLIST.at(dirPtr).second == 0)
		return -1;
	DIR dir = DIRLIST.at(dirPtr).first;	//��Ŀ¼
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
	DIR dir = DIRLIST.at(dirPtr).first;	//��Ŀ¼
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
			else cerr<<"�������"<<endl;

		}
		else if(cmdName == "cd") {
			if(paraList.size() == 0) CUPTR = 0;
			else if(paraList.size() == 1) CUPTR = cd(CUPTR,paraList[0]);
			else cerr<<"�������"<<endl;
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