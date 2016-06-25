/*-----------------------------------------------------------|
| Copyright (C) 2016 Yuan-Yen Tai, Hongchul Choi,            |
|                    Jian-Xin Zhu                            |
|                                                            |
| This file is distributed under the terms of the GNU        |
| General Public License. See the file `LICENSE' in          |
| the root directory of the present distribution, or         |
| http://www.gnu.org/copyleft/gpl.txt .                      |
|                                                            |
|-----------------------------------------------------------*/

//
//  string_func.hpp
//  TBM^3
//
//  Created by Yuan Yen Tai on 7/15/15.
//

#ifndef _string_func_h
#define _string_func_h

void removeSpace(std::string & str) {
	if (str.size() > 0) {
		string tmpStr = "";
		for( unsigned i=0 ; i<str.size() ; i++){
			if (str[i] != ' ' and str[i] != '\t') {
				tmpStr += str[i];
			}
		}
		str = tmpStr;
	}
}

// The string replace function.
void			replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

void			deleteComment(std::string& str, char comment = '%'){
	string tmpStr = "";
	for(auto & c: str){
		if( c!= comment) tmpStr.push_back(c);
		else{
			str = tmpStr;
			return;
		}
	}
}

// The string split function.
deque<string>	split(string line, string delimiter) {
	replaceAll(line, "\t", " ");
    std::deque<std::string> content;
    
    size_t pos = 0;
    std::string token;
    while ((pos = line.find(delimiter)) != std::string::npos) {
        token = line.substr(0, pos);
        if (token.size()>0) content.push_back(token);
        line.erase(0, pos + delimiter.length());
    }
    if (line.size()>0) content.push_back(line);
    
    return content;
}

// The string split function.
deque<string>	splitByIntNumber(string line) {
	replaceAll(line, "\t", " ");
    std::deque<std::string> content;
	line += " ";
	
	string word = "";
	for( unsigned i=0 ; i<line.size()-1 ; i++){
		char c = line[i];
		char cnext = line[i+1];
		
		if		( (c>= '0' and c<='9') or c=='.'){
			word.push_back(c);
			if( (cnext < '0' or cnext > '9' or cnext == ' ') and cnext != '.'){
				content.push_back(word);
				word = "";
			}
		}
		else if	( (c < '0' or c > '9') and c!='.'){
			word.push_back(c);
			if( (cnext >= '0' and cnext <='9') or cnext == ' '){
				content.push_back(word);
				word = "";
			}
		}
	}
	
    return content;
}

// eifjofejfojsef

/* String-Variable convertors. */
int				StrToInt(string Str)	{
    istringstream iss(Str);
    int var;
    iss>>var;
    return var;
}
string			IntToStr(int val)		{
    ostringstream oss;
    oss<<val;
    return oss.str();
}
double			StrToDouble(string Str) {
	istringstream iss(Str);
	double var;
	iss>>var;
	return var;
}
string			DoubleToStr(double val)	{
    ostringstream oss;
    oss<<val;
    return oss.str();
}
x_var			StrToComplex(string Str){
	replaceAll(Str, " ", "");
	
	x_var ret_var;
	if (Str[0] == '(' and Str[Str.size()-1] == ')') {
		Str[0] = ' ';
		Str[Str.size()-1] = ' ';
		auto var = split(Str, ",");
		if		( var.size() == 1) ret_var = StrToDouble(var[0]);
		else if	( var.size() == 2) ret_var = x_var( StrToDouble(var[0]), StrToDouble(var[1]));
		else{
			ErrorMessage("Error, string:"+Str+" is not a valid from of complex variable.");
		}
		
	} else {
		ret_var = StrToDouble(Str);
	}
    return ret_var;
}
bool			IsIntStr(string Str)	{
	bool retVal = true;
	for(auto &c: Str) retVal = retVal and ( (c>='0' and c<='9') or c=='-' or c=='+');
	return retVal;
}
bool			IsFloatStr(string Str)	{
	bool retVal = true;
	for(auto &c: Str) retVal = retVal and ( (c>='0' and c<='9') or c=='-' or c=='+' or c=='.');
	return retVal;
}

/* Counting how many words in one string. */
int				WordCount(string token, string line){
	
	int count=0;
	
    size_t pos = 0;
    while ((pos = line.find(token)) != std::string::npos) {
        line.erase(0, pos + token.length());
		count++;
    }
	
	
	return count;
}

/* Convert string type: "(1,2), 2,3, (3,4)" into a 1xN x_mat.*/
x_mat	&		StrToXVec(string Str)	{
	bool xflag = false;
	vector<pair<bool, string> > subList;
	
	string tmpStr = "";
	for( auto & c : Str ){
		
		if( c == '(' ){ xflag = true;	tmpStr.push_back(c);	continue;}
		if( c == ')' ){ xflag = false;	tmpStr.push_back(c);	continue;}
		
		if( c!= ' ' and c!= '\t') {
			// Complex number goes here.
			if(xflag)	{ tmpStr.push_back(c); }
			else		{
				if( c!=',') tmpStr.push_back(c);
				else		tmpStr.push_back('@');
			}
		}
	}
	auto xList = split(tmpStr, "@");
	
	x_mat *ret = new x_mat(1,xList.size());
	for( unsigned ii=0 ; ii<xList.size() ; ii++){
		auto & xStr = xList[ii];
		ret->index(ii) = StrToComplex(xStr);
	}
	
	return *ret;
}

/* Translate "+" or "-" into +1 or -1. */
double			PMStr(string str){ // return +1 or -1 for a given +
	if(str == "+") return +1;
	if(str == "-") return -1;
	return 0;
}

// This function will extract the atom informations, for:
// 1. Number of orbitals.
// 2. If the atom has spin degree of freedom.
vector<string>	analyze_atom_label(string label){
	vector<string> lab_list;
	string L_num  = "";
	string L_spin = "";
	for (unsigned i=0; i<label.size(); i++) {
		if ( label[i] >= '0' and label[i] <= '9') {
			L_num.push_back(label[i]);
		}
		else{
			L_spin.push_back(label[i]);
		}
	}
	lab_list.push_back(L_num);
	lab_list.push_back(L_spin);
	return lab_list;
}

#endif











