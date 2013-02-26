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

#define Q0(s)	"\"" #s "\""
#define Q(s)	",\"" #s "\""
#define QP0(q, r)	Q0(q) ":" Q0(r)
#define QP(q, r)	Q(q) ":" Q0(r)


string
quote(string s)
{
  // first, if 'true', 'false', or 'null', no quoting needed
  if (s == "true" || s == "false" || s == "null")
    return s;

  // if it doesn't convert, quote it; otherwise return it
  unsigned long long ull;
  try {
    ull = lexical_cast<unsigned long long>(s);
    return s;
  } catch(bad_lexical_cast&){
    return "\"" + s + "\"";
  }
}

string comma_unless(bool &first)
{
  if (first) {
    first = false;
    return "";
  } else {
    return ",";
  }
}

void
transform_sig_to_json(string &sig, string *ret)
{
  stringstream ss(sig);
  string word;
  stringstream argjson, descjson;
  bool firstword = true;

  // put whole command signature in an array
  // elements are: "name", meaning "the typeless name that means a literal"
  // an object {} with key:value pairs representing an argument
  // 
  descjson << "[";
  int argnum = 0;
  while (getline(ss, word, ' ')) {
    argnum++;
    // if no , or =, must be a plain word to put out
    if (word.find_first_of(",=") == string::npos) {
      descjson << comma_unless(firstword) << quote(word);
      continue;
    }
    // snarf up all the key=val,key=val pairs, put 'em in a dict
    stringstream argdesc(word);
    string t, name;
    bool firstarg = true;
    while (getline(argdesc, t, ',')) {
      int pos;
      argjson << comma_unless(firstarg);
      // key=value or key[:True] (the :True is implied)
      // name="name" means arg dict will be titled 'name'
      pos = t.find('=');
      string key, val;
      if (pos != string::npos) {
	key = t.substr(0, pos);
	val = t.substr(pos+1);
      } else {
        key = t;
        val = "true";
      }
      argjson << quote(key) << ":" << quote(val);
    }
    descjson << comma_unless(firstword) << "{" << argjson.str() << "}";
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

#if 0
  string d1("\"cmd\":[\"osd\",\"tell\",\"id\":{\"type\":\"CephID\",\"n\":1}]");
  string d2(Q0(cmd) ":[" Q0(osd) Q(tell) Q(id) ":{" QP0(type, CephID) Q(n) ":1" "}}");
  string d3("osd tell type=CephID,name=id,n=1");

  cout << d1 << endl;
  cout << d2 << endl;
#endif
  string s;
#if 0
  transform_sig_to_json(d3, &s);
  cout << s << endl;
#endif
  string sigs[] = {
    "admin type=CephSocketpath,name=admin-socket type=CephString,name=cmd,n=N",
    "osd stat",
    "osd dump",
    "osd tree",
    "osd ls",
    "osd getmap",
    "osd getcrushmap",
    "osd getmaxosd",
    "osd tell type=CephInt,name=osdnum",
    "osd map type=CephPoolname,name=pool type=CephObjectname,name=obj",
    "osd scrub type=CephInt,name=osdnum",
    "osd deep-scrub type=CephInt,name=osdnum",
    "osd repair type=CephInt,name=osdnum",
    "osd lspools type=CephInt,name=auid,req=false",
    "osd blacklist ls",
    "osd blacklist add type=CephEntityAddr,name=addr type=CephFloat,range=0.0|1.0e7,req=false,name=until",
    "osd blacklist rm type=CephEntityAddr,name=addr",
    "osd crush rule type=CephChoices,strings=list|ls,name=listorls",
    "osd crush rule dump",
    "osd crush dump",
    "osd crush set type=CephInt,name=id type=CephName,name=osdname,req=false type=CephFloat,name=weight type=CephString,n=N,name=loc",
    "osd crush create-or-move type=CephInt,name=id type=CephFloat,name=weight type=CephString,n=N,name=loc",
    "osd crush move type=CephString,name=name type=CephString,n=N,name=loc",
    "osd crush type=CephChoices,strings=rm|remove,name=op type=CephInt,name=id",
    "osd crush reweight type=CephString,name=name type=CephFloat,name=weight",
    "osd tunables type=CephChoices,strings=legacy|argonaut|bobtail|optimal|default,name=tunable-scheme",
    "osd rule create-simple type=CephString,name=name type=CephString,name=root type=CephString,name=type",
    "osd rule rm type=CephString,name=name",
    "osd setcrushmap",
    "osd crush set",
    "osd setmaxosd type=CephInt,name=newmax",
    "osd type=CephChoices,strings=pause|unpause,name=op",
    "osd type=CephChoices,strings=set|unset,name=op type=CephChoices,strings=pause|noup|nodown|noout|noin|nobackfill|norecover,name=val",
    "osd cluster_snap",
    "osd down type=CephInt,name=id,n=N",
    "osd type=CephChoices,strings=down|out|in,name=op type=CephInt,name=id,n=N",
    "osd reweight type=CephInt,name=id type=CephFloat,name=weight,range=0.0|1.0",
    "osd lost type=CephInt,name=id --yes-i-really-mean-it",
    "osd create type=CephUUID,name=uuid",
    "osd rm type=CephInt,name=id,n=N",
    "osd pool type=CephChoices,strings=mksnap|rmsnap,name=op type=CephPoolname,name=pool type=CephString,name=snap",
    "osd pool create type=CephPoolname,name=pool type=CephInt,name=pg_num type=CephInt,name=pgp_num,req=false",
    "osd pool delete type=CephPoolname,name=pool type=CephPoolname,name=pool2 --yes-i-really-really-mean-it",
    "osd pool rename type=CephPoolname,name=from type=CephPoolname,name=to",
    "osd pool type=CephChoices,strings=set|get,name=op type=CephPoolname,name=pool type=CephChoices,strings=size|min_size|crash_replay_interval|pg_num|pgp_num|crush_ruleset,name=var, type=CephInt,name=n",
    "osd reweight-by-utilization type=CephInt,name=oload,range=100|10000",
    "osd thrash type=CephInt,name=num_epochs",
    ""
  };

  string *p;
  bool firstsig = true;
  int signum = 0;
  bool standalone = (!args.empty() && (args[0] == "-s" || args[0] == "--standalone"));
  if (standalone)
    cout << "{" << endl;
  for (p = sigs; p->length() != 0; p++) {
    stringstream cmdstr;
    signum++;
    if (standalone) {
      cmdstr << "cmd";
      cmdstr.width(2);
      cmdstr.fill('0');
      cmdstr << signum;
    }
    transform_sig_to_json(*p, &s);
    if (standalone)
      cout << comma_unless(firstsig) << endl << quote(cmdstr.str()) << ":" << s;
    else
      cout << s << endl;
  }
  if (standalone)
    cout << "}" << endl;
}
