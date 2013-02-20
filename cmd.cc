// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using boost::bad_lexical_cast;
using namespace std;


// {'name':numeric value, 'name2':'string value'}

#if 0

#define NS(s, t)	"\"" #s "\":\"" #t "\""
#define NI(s, t)	"\"" #s "\":" #t 
#define OSDS(s) { "\"osd\":\"" #s "\"" }

const char *a[][10] =
{
  OSDS(stat),
  OSDS(dump),
  OSDS(tree),
  OSDS(ls),
  OSDS(dump),
  OSDS(getmap),
  OSDS(getcrushmap),
  OSDS(getmaxosdmap),
  { NS(c1, osd), NS(c2, tell), NS(type, CephID), NS(name, id), NS(required, True), NI(num, 1), NULL},
  NULL
};
  //{"c1: osd", "c2: tell", "{type:CephID}, {name:id}, {required:True}, {num:1}"},
  //{"c1: osd", "c2: find", "{type:CephInt}, {name:n}, {required:True}, {num:1}"},
  //{"c1: osd", "c2: map", "{type:CephPool}, {name:pool}, {required:True}, {num:1}"},

int
main(int argc, char **argv)
{
  vector<string>args;
  for (int i = 1; i < argc; i++)
    args.push_back(argv[i]);

  cout << "{" << endl;
  bool firsti = true;
  for (int i = 0; i < sizeof(a); i++) {
    if (a[i][0] == NULL)
      break;
    if (firsti)
      firsti = false;
    else
      cout << ",";

    cout << "\"cmd" << i << "\":";
    bool firstj = true;
    for (int j = 0; j < sizeof(a[i]); j++) {
      if (a[i][j] == NULL)
	break;
      if (firstj)
	firstj = false;
      else
	cout << ",";
      cout << a[i][j];
    }
    cout << "}";
  }
  cout << "}" << endl;
}
# else

#define Q0(s)	"\"" #s "\""
#define Q(s)	",\"" #s "\""
#define QP0(q, r)	Q0(q) ":" Q0(r)
#define QP(q, r)	Q(q) ":" Q0(r)


string
q(string s)
{
  // if it doesn't convert, quote it; otherwise return it
  unsigned long long ull;
  try {
    ull = lexical_cast<unsigned long long>(s);
  } catch(bad_lexical_cast&){
    return "\"" + s + "\"";
  }
  return s;
}

string
transform_sig_to_json(string &sig, string *ret)
{
  stringstream ss(sig);
  string word;
  stringstream argjson, descjson;
  bool firstword = true;

  // put whole command signature in an array called 'cmd'
  descjson << "cmd:[";
  while (getline(ss, word, ' ')) {
    // if no , or =, must be a plain word to put out
    if (word.find_first_of(",=") == string::npos) {
      if (firstword) {
	firstword = false;
      } else {
	descjson << ",";
      }
      descjson << q(word);
      continue;
    }
    // snarf up all the key=val,key=val pairs, put 'em in a dict
    stringstream argdesc(word);
    string t, name;
    bool firstkeyval = true;
    while (getline(argdesc, t, ',')) {
      int pos;
      if (firstkeyval) {
	firstkeyval = false;
      } else {
	argjson << ',';
      }
      // key=value or key[:True] (the :True is implied)
      // name="name" means arg dict will be titled 'name'
      pos = t.find('=');
      if (pos != string::npos) {
	string key = t.substr(0, pos);
	string val = t.substr(pos+1);
	if (key == "name")
	  name = val;
	argjson << q(key) << ":" << q(val);
      } else {
	argjson << q(t) << ":\"True\"";
      }
    }
    descjson << q(name) << ":{" << argjson.str() << "}";
    argjson.str("");
  }
  descjson << "]";
  *ret = descjson.str();
}

int
main(int argc, char **argv)
{
  vector<string>args;
  for (int i = 1; i < argc; i++)
    args.push_back(argv[i]);

  string d1("\"cmd\":[\"osd\",\"tell\",\"id\":{\"type\":\"CephID\",\"req\":\"True\",\"n\":1}]");
  string d2(Q0(cmd) ":[" Q0(osd) Q(tell) Q(id) ":{" QP0(type, CephID) QP(req, True) Q(n) ":1" "}}");
  string d3("osd tell type=CephID,name=id,req,n=1");


  cout << d1 << endl;
  cout << d2 << endl;
  string s;
  transform_sig_to_json(d3, &s);
  cout << s << endl;
}

#endif
