#include <openssl/hmac.h>
#include <openssl/evp.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const string SECRET_KEY = "class_demo_secret_key";
const string TAG_FILE = "message.tag";

vector<unsigned char> readFileBytes(const string& filename) {
    ifstream file(filename, ios::binary);

    if (!file) {
        cerr << "Error: could not open file: " << filename << endl;
        exit(1);
    }

    vector<unsigned char> data;
    char ch;

    while (file.get(ch)) {
        data.push_back(static_cast<unsigned char>(ch));
    }

    return data;
}

string bytesToHex(const unsigned char* bytes, unsigned int length) {
    stringstream ss;

    for (unsigned int i = 0; i < length; i++) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(bytes[i]);
    }

    return ss.str();
}

string createHmac(const vector<unsigned char>& data) {
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int resultLength = 0;

    HMAC(
        EVP_sha256(),
        SECRET_KEY.c_str(),
        static_cast<int>(SECRET_KEY.length()),
        data.data(),
        data.size(),
        result,
        &resultLength
    );

    return bytesToHex(result, resultLength);
}

void saveTag(const string& tag) {
    ofstream file(TAG_FILE);

    if (!file) {
        cerr << "Error: could not write tag file." << endl;
        exit(1);
    }

    file << tag << endl;
}

string readTag() {
    ifstream file(TAG_FILE);

    if (!file) {
        cerr << "Error: could not open tag file. Run create mode first." << endl;
        exit(1);
    }

    string tag;
    getline(file, tag);

    return tag;
}

void createMode(const string& filename) {
    vector<unsigned char> data = readFileBytes(filename);
    string tag = createHmac(data);

    saveTag(tag);

    cout << "Authentication tag created." << endl;
    cout << "File: " << filename << endl;
    cout << "Tag saved to: " << TAG_FILE << endl;
    cout << "Tag: " << tag << endl;
}

void verifyMode(const string& filename) {
    vector<unsigned char> data = readFileBytes(filename);

    string savedTag = readTag();
    string newTag = createHmac(data);

    cout << "Saved tag:      " << savedTag << endl;
    cout << "Calculated tag: " << newTag << endl;

    if (savedTag == newTag) {
        cout << endl;
        cout << "Verification successful." << endl;
        cout << "File has not been changed." << endl;
    }
    else {
        cout << endl;
        cout << "Verification failed." << endl;
        cout << "File may have been modified." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage:" << endl;
        cout << "  ./hmac_demo create <filename>" << endl;
        cout << "  ./hmac_demo verify <filename>" << endl;
        return 1;
    }

    string mode = argv[1];
    string filename = argv[2];

    if (mode == "create") {
        createMode(filename);
    }
    else if (mode == "verify") {
        verifyMode(filename);
    }
    else {
        cout << "Invalid mode." << endl;
        cout << "Use create or verify." << endl;
        return 1;
    }

    return 0;
}