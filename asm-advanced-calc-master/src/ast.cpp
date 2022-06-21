#include "ast.h"
#include <iostream>
#include <sstream>
#include <set>
#include "asm.h"

const char * floatTemps[] = {"$f0",
                            "$f1",
                            "$f2",
                            "$f3",
                            "$f4",
                            "$f5",
                            "$f6",
                            "$f7",
                            "$f8",
                            "$f9",
                            "$f10",
                            "$f11",
                            "$f12",
                            "$f13",
                            "$f14",
                            "$f15",
                            "$f16",
                            "$f17",
                            "$f18",
                            "$f19",
                            "$f20",
                            "$f21",
                            "$f22",
                            "$f23",
                            "$f24",
                            "$f25",
                            "$f26",
                            "$f27",
                            "$f28",
                            "$f29",
                            "$f30",
                            "$f31"
                        };

#define FLOAT_TEMP_COUNT 32
set<string> intTempMap;
set<string> floatTempMap;

extern Asm assemblyFile;

int globalStackPointer = 0;

string getFloatTemp(){
    for (int i = 0; i < FLOAT_TEMP_COUNT; i++)
    {
        if(floatTempMap.find(floatTemps[i]) == floatTempMap.end()){
            floatTempMap.insert(floatTemps[i]);
            return string(floatTemps[i]);
        }
    }
    cout<<"No more float registers!"<<endl;
    return "";
}

void releaseFloatTemp(string temp){
    floatTempMap.erase(temp);
}

void FloatExpr::genCode(Code &code){

    string floatTmp = getFloatTemp();
    code.place = floatTmp;
    stringstream ss;
    ss << "li.s " << floatTmp << ", " << this->number <<endl;
    code.code = ss.str();

}

void SubExpr::genCode(Code &code){

    Code leftCode;
    Code rightCode;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    code.code = leftCode.code + "\n" + rightCode.code + "\n";
    string newTemp = getFloatTemp();
    code.code = "sub.s " + newTemp + ", " + leftCode.place + ", " + rightCode.place + "\n";
    releaseFloatTemp(leftCode.place);
    releaseFloatTemp(rightCode.place);
    code.place = newTemp;

}

void DivExpr::genCode(Code &code){

    Code leftCode;
    Code rightCode;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    code.code = leftCode.code + "\n" + rightCode.code + "\n";
    string temp = getFloatTemp();
    code.code = "div.s " + temp + ", " + leftCode.place + ", " + rightCode.place + "\n";
    releaseFloatTemp(leftCode.place);
    releaseFloatTemp(rightCode.place);
    code.place = temp;

}

void IdExpr::genCode(Code &code){

    if(floatTempMap.find(this->id) == floatTempMap.end()){
        string temp = getFloatTemp();
        code.place = temp;
        code.code = "l.s " + temp + ", " + this->id + "\n";
    }
    
}

string ExprStatement::genCode(){
    Code code;
    this->expr->genCode(code);
    releaseFloatTemp(code.place);
    return code.code;
}

string IfStatement::genCode(){

    string endIf = ("endif");
    Code code;
    this->conditionalExpr->genCode(code);
    stringstream ss;
    ss << code.code <<endl;
    ss << "beqz " << code.place << ", " << endIf <<endl;
    
    return ss.str();

}

void MethodInvocationExpr::genCode(Code &code){

}

string AssignationStatement::genCode(){
   return "Assignation statement code generation\n";
}

void GteExpr::genCode(Code &code){

    Code leftCode;
    Code rightCode;
    stringstream ss;
    ss << code.code << endl;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code <<endl<< rightCode.code <<endl;
    string temp= getFloatTemp();
    ss << "c.gt.s " << leftCode.place << " , " << rightCode.place <<endl;
    releaseFloatTemp(rightCode.place);
    releaseFloatTemp(leftCode.place);
    code.place= temp;
    code.code= ss.str();

}

void LteExpr::genCode(Code &code){

    Code leftCode;
    Code rightCode;
    stringstream ss;
    ss << code.code << endl;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code <<endl<< rightCode.code <<endl;
    string temp= getFloatTemp();
    ss << "c.lt.s " << leftCode.place<<" , "<< rightCode.place <<endl;
    releaseFloatTemp(rightCode.place);
    releaseFloatTemp(leftCode.place);
    code.place= temp;
    code.code= ss.str();

}

void EqExpr::genCode(Code &code){

    Code leftCode;
    Code rightCode;
    stringstream ss;
    ss << code.code <<endl;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code <<endl<< rightCode.code <<endl;
    string temp = getFloatTemp();
    ss << "c.eq.s " << leftCode.place << ", " << rightCode.place <<endl;
    releaseFloatTemp(leftCode.place);
    releaseFloatTemp(rightCode.place);
    code.place = temp;
    code.code = ss.str();

}

void ReadFloatExpr::genCode(Code &code){
    
}

string PrintStatement::genCode(){

    list<Expr *>::iterator lista = this->expressions.begin();
    Code code;

    while(lista != this->expressions.end()){
        (*lista)->genCode(code);
        lista++;
    }

    releaseFloatTemp(code.place);
    stringstream ss;
    ss << code.code <<endl;
    ss << "mov.s $f12, " << code.place <<endl<< "li $v0, 2" <<endl<< "syscall" <<endl;
    return ss.str();
    
}

string ReturnStatement::genCode(){

    Code code;
    this->expr->genCode(code);
    releaseFloatTemp(code.place);
    stringstream ss;
    ss << code.code << endl;
    ss << "mf1c $v0, " << code.place <<endl;
    return ss.str();
    
}

string MethodDefinitionStatement::genCode(){    
    return "Method definition code generation\n";
}