#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "b64.h"
#include <sys/stat.h>
#include "utils.h"
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
using namespace std;

int editDistance(const char* first, const char* second, int length) {
   int distance = 0;
   for (int i = 0; i < length; i ++ ) {
      char x = first[i] ^ second[i];
      for (int j = 0; j < 8; j++) {
         if (x & (1 << j)) {
            distance++;
         }
      }
   }
   return distance;
}

char * base64readFile(const char * filename, int *totalSize) {
   struct stat fs;
   stat(filename, &fs);
   int filesize = fs.st_size;
   char * data = (char*) malloc(sizeof(char) * filesize * 3 / 4 );
   size_t offset = 0;

   FILE* input = fopen(filename, "r");
   char * buffer = NULL;
   size_t size = 0;
   int nread;
   while ((nread = getline(&buffer, &size, input)) > 0) {
      char * decoded;
      int dlen = b64tobin(buffer, nread, &decoded);
      memcpy(data + offset, decoded, dlen);
      offset += dlen;
      free(decoded);
      //free(buffer);
      //size = 0;
      //buffer = NULL;
   }
   if (buffer != NULL) free(buffer);
   fclose(input);
   *totalSize = (int) offset;
   return data;
}

std::vector<std::pair<std::string, std::string>> parseToken(const string & token) {
   vector<pair<string, string>> pairs;

   vector<string> tokens;
   char* dup = strdup(token.c_str());
   char * tok = std::strtok(dup, "&");
   while (tok != NULL) {
      tokens.push_back(tok);
      tok = std::strtok(NULL, "&");
   }
   free(dup);

   for (auto t : tokens) {
      int pos = t.find("=");
      pairs.push_back(std::make_pair<string,string>(
               string(t.c_str(), 0, pos), string(t.c_str(), pos+1, t.size())));
   }
   return pairs;
}

string profile_for(const string &email) {
   stringstream ss;
   ss << "email=";
   for (size_t i =0; i < email.size(); i++) {
      switch(email.at(i)) {
         case '=':
            ss << "\%3D";
            break;
         case '&':
            ss << "%26";
            break;
         default:
            ss << email.at(i);
      }
   }
   ss << "&role=user&uid=10";
   return ss.str();
}