#include <iostream>
#include <pthread.h>
#include <algorithm>
#include <string>
#include <vector>
#include <set>

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

struct DocumentContent {
    int id;
    vector<string> words;
};

struct Document {
    int id;
    int relevance;
};

//5.Adding a document to the databases.
void AddDocument(vector<DocumentContent>& documents, int id_document,
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
int RelevantDocument(const DocumentContent document, const set<string>& search_query ) {

    int relevant { };
    set<string> query { };
    for( const string& word : document.words) {
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
vector<Document>FindAllDocument( const vector<DocumentContent>& documents,
        const set<string>& query_words ) {

    vector<Document> rezult { };
    for( const auto& document : documents ) {
        int relevant = RelevantDocument(document, query_words);
        if( relevant > 0 ) {
            rezult.push_back({document.id, relevant});
        }
    }

    return rezult;
}

bool SortLargestToSmallest( const Document& lhs, const Document& rhs ) {
    return lhs.relevance < rhs.relevance;
}

//9.Find TOP document, get id and relevance 
vector<Document>FindTopDocument( const vector<DocumentContent>& documents,
        const set<string>& stop_words, const string& raw_query ) {
    vector<Document>rezult_top { };
    set<string> query_words = ParseQuery(raw_query, stop_words);
    for( const auto& [id, relevance] : FindAllDocument(documents, query_words) ) {
        rezult_top.push_back({id, relevance});
    }

    sort(rezult_top.begin(), rezult_top.end(), SortLargestToSmallest);
    if( rezult_top.size() > MAX_RESULT_DOCUMENT_COUNT ) {
        rezult_top.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    return rezult_top;
}

class SearchServer {
public:
        void AddDocument(int document_id, const string& document) {
            const vector<string> words = SplitInToWordsNoStop(document);
            documents_.push_back({document_id, words});

        }

private:
        struct DocumentContent {
            int id = 0;
            vector<string> words;
        };

        vector<DocumentContent> documents_;
        set<string> stop_words_;

        bool IsStopWord( const string& word ) const {
            return stop_words_.count(word) > 0;
        }

        vector<string> SplitInToWordsNoStop( const string& text ) const {
            vector<string> words { };
            for( const string& word : SplitInToWords(text) ) {
                if( !IsStopWord(word) ) {
                    words.push_back(word);
                }
            }
            return words;
        }

        set<string> ParseQuery( const string& text ) {
            set<string> query_words { };
            for( const string& word : SplitInToWordsNoStop(text) ) {
                query_words.insert(word);
            }
            return query_words;
        }

        static int MatchDocument( const DocumentContent& content, const set<string>& query_words ) {

            if( query_words.empty() ) {
                return 0;
            }
            set<string> matched_words { };
            for( const string& word : content.words ) {
                if( matched_words.count(word) != 0 ) {
                    continue;
                }
                if( query_words.count(word) !=0 ) {
                    matched_words.insert(word);
                }
            }
            return static_cast<int>(matched_words.size());
        }
};

int main() {

    vector<DocumentContent>databases { };
    cout << "enter stop-word" << endl;
    set<string> stop_words = CreateStopWords(ReadUserString());
    cout << "enter count add document" << endl; 
    int count = ReadUserNumber();

    for( int i = -1; i <= count -1; ++i ) {
        cout << i << endl;

        AddDocument( databases, i, stop_words, ReadUserString() );
    }

    cout << "enter search query" << endl;
    string query = ReadUserString();

    for( auto& [id, value] : FindTopDocument(databases, stop_words, query) ) {
        cout << "id - " << id << " relevant - " << value << endl;
    }

}
