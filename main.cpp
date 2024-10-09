#include <iostream>
#include <pthread.h>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

//1.Read user input(string)
string ReadUserString() {

    string input_user { };
    getline( cin, input_user );
    return input_user;
}

//1.Read user input(int)
int ReadUserNumber() {

    int quatity_documents { };
    cin >> quatity_documents;
    return quatity_documents;
}

//2.Splitting a string into words
vector<string> SplitInToWords( const string& text ) {

    vector<string> words { };
    string word { };

    for( char i : text ) {
        if( i == ' ' ) {
            if( !word.empty() ) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += i;
        }
    }

    if( !word.empty() ) {
        words.push_back(word);
        word.clear();
    }

    return words;
}

//3.Create stop-words
set<string> CreateStopWords( const string& text ) {
    
    set<string> stop_words { };
    for( const string& word : SplitInToWords(text) ) {
        stop_words.insert(word);
    }
    
    return stop_words;
}

//4.Forming a query, excluding stop words
vector<string> FormedQuery( const string& text, const set<string>& stop_words ) {
    
    vector<string> formed_query { };
    for( const string& word : SplitInToWords(text) ) {
            if( stop_words.count(word) == 0 ) {
                formed_query.push_back(word);
            }
        }

    return formed_query;
}

//5.Adding a document to the databases.
void AddDocument(vector<pair<int, vector<string>>>& documents, int id_document,
    set<string>& stop_words, const string& document ) {

    vector<string> query_words =  FormedQuery(document, stop_words);
    documents.push_back({id_document, query_words});
}

//6. Remove duplicate words from the query.
set<string> ParseQuery( const string& text, const set<string>& stop_words ) {

    set<string> query_words { };
    for( const string& word : FormedQuery(text, stop_words) ) {
        query_words.insert(word);
    }

    return query_words;
}

//7.Document relevance
int RelevantDocument(const pair<int, vector<string>>& document, const set<string>& search_query ) {

    int relevant { };
    set<string> query { };
    for( const string& word : document.second ) {
        query.insert(word);
    }

    for( const string& doc_query : query ) {
        for( const string& insert_query : search_query ) {
            if( doc_query == insert_query ) {
                ++relevant;
            }
        }
    }

    return relevant;
}

//8.Find ALL document, get relevance and id
vector<pair<int, int>>FindAllDocument( const vector<pair<int, vector<string>>>& documents,
        const set<string>& query_words ) {

    vector<pair<int, int>> rezult { };
    for( const auto& document : documents ) {
        int relevant = RelevantDocument(document, query_words);
        if( relevant > 0 ) {
            rezult.push_back({relevant, document.first});
        }
    }

    return rezult;
}

vector<pair<int, int>>FindTopDocument( const vector<pair<int, vector<string>>>& documents,
        const set<string>& stop_words, const string& raw_query ) {
    vector<pair<int, int>> rezult_top { };
    set<string> query_words = ParseQuery(raw_query, stop_words);
    for( const auto& [relevant, id] : FindAllDocument(documents, query_words) ) {
        rezult_top.push_back({id, relevant});
    }

    if( rezult_top.size() > MAX_RESULT_DOCUMENT_COUNT ) {
        rezult_top.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    return rezult_top;
}



int main() {
    vector<pair<int, vector<string>>> databases { };
    cout << "enter stop-word" << endl;
    set<string> stop_words = CreateStopWords(ReadUserString());
    //TEST
    //cout << "zapros" << endl;
    //set<string> input_zap = ParseQuery( ReadUserString(), stop_words );
    //set<string> qu = ParseQuery( input_zap, stop_words );
    //TEST
    cout << "enter count add document" << endl; 
    int count = ReadUserNumber();


    for( int i = 0; i <= count; ++i ) {
        cout << i << endl;

        AddDocument( databases, i, stop_words, ReadUserString() );
    }
    cout << "enter search query" << endl;
    string query = ReadUserString();
    for( auto& [id, value] : FindTopDocument(databases, stop_words, query) ) {
        cout << "id - " << id << " relevant - " << value << endl;
    }

/*
    TEST
    for( const auto& document : databases ) {
        cout << document.first << " - ";
        int z = RelevantDocument(document, input_zap);
        cout << "relevant = " << z;

        for( const auto& str : document.second ) {
            cout << " - " << str;
        }
        cout << endl;
    }
    TEST
 */   


}
