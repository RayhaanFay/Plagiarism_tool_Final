#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

string readFile(string filename)
{
    ifstream file(filename);

    if (!file)
    {
        cout << "Error: Could not open " << filename << endl;
        return "";
    }

    string text;
    string line;

    while (getline(file, line))
    {
        text += line + " ";
    }

    file.close();

    return text;
}


string preprocess(string text)
{
    string result = "";

    for (char c : text)
    {
        if (isalnum(c) || c == ' ')
        {
            result += tolower(c);
        }
        else
        {
            result += ' ';
        }
    }

    return result;
}


vector<int> buildLPS(string pattern)
{
    vector<int> lps(pattern.length(), 0);

    int length = 0;
    int i = 1;

    while (i < pattern.length())
    {
        if (pattern[i] == pattern[length])
        {
            length++;
            lps[i] = length;
            i++;
        }
        else
        {
            if (length != 0)
            {
                length = lps[length - 1];
            }
            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}

int KMP(string text, string pattern)
{
    if (pattern.empty())
        return 0;

    vector<int> lps = buildLPS(pattern);

    int i = 0;
    int j = 0;
    int count = 0;

    while (i < text.length())
    {
        if (text[i] == pattern[j])
        {
            i++;
            j++;
        }

        if (j == pattern.length())
        {
            count++;
            j = lps[j - 1];
        }
        else if (i < text.length() && text[i] != pattern[j])
        {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }

    return count;
}


int RabinKarp(string text, string pattern)
{
    if (pattern.empty() || pattern.length() > text.length())
        return 0;

    const int base = 256;
    const int prime = 101;

    int m = pattern.length();
    int n = text.length();

    int patternHash = 0;
    int textHash = 0;
    int h = 1;

    for (int i = 0; i < m - 1; i++)
    {
        h = (h * base) % prime;
    }

    for (int i = 0; i < m; i++)
    {
        patternHash =
            (base * patternHash + pattern[i]) % prime;

        textHash =
            (base * textHash + text[i]) % prime;
    }

    int count = 0;

    for (int i = 0; i <= n - m; i++)
    {
        if (patternHash == textHash)
        {
            bool match = true;

            for (int j = 0; j < m; j++)
            {
                if (text[i + j] != pattern[j])
                {
                    match = false;
                    break;
                }
            }

            if (match)
                count++;
        }

        if (i < n - m)
        {
            textHash =
                (base * (textHash - text[i] * h)
                + text[i + m]) % prime;

            if (textHash < 0)
                textHash += prime;
        }
    }

    return count;
}


vector<string> getWords(string text)
{
    vector<string> words;

    stringstream ss(text);
    string word;

    while (ss >> word)
    {
        words.push_back(word);
    }

    return words;
}


vector<string> findMatchingChunks(
    string doc1,
    string doc2,
    int &kmpMatches,
    int &rabinMatches)
{
    vector<string> words = getWords(doc1);

    vector<string> matches;

    kmpMatches = 0;
    rabinMatches = 0;

    const int CHUNK_SIZE = 5;

    for (int i = 0; i + CHUNK_SIZE <= words.size(); i += CHUNK_SIZE)
    {
        string chunk = "";

        for (int j = i; j < i + CHUNK_SIZE; j++)
        {
            chunk += words[j] + " ";
        }

        chunk.pop_back();

        int kmpResult = KMP(doc2, chunk);
        int rabinResult = RabinKarp(doc2, chunk);

        if (kmpResult > 0)
        {
            kmpMatches++;
        }

        if (rabinResult > 0)
        {
            rabinMatches++;
        }

        if (kmpResult > 0 || rabinResult > 0)
        {
            matches.push_back(chunk);
        }
    }

    return matches;
}


// ============================================================
// 8. CALCULATE SIMILARITY
//
// Prototype formula based on matching chunks.
// ============================================================

double calculateSimilarity(
    int matchingChunks,
    int totalChunks)
{
    if (totalChunks == 0)
        return 0;

    return ((double)matchingChunks / totalChunks) * 100;
}


// ============================================================
// 9. GENERATE REPORT
// ============================================================

void generateReport(
    vector<string> matches,
    double similarity,
    int kmpMatches,
    int rabinMatches)
{
    ofstream report("reports/plagiarism_report.txt");

    report << "============================================\n";
    report << "        PLAGIARISM DETECTION REPORT\n";
    report << "============================================\n\n";

    report << fixed << setprecision(2);

    report << "Similarity Percentage : "
           << similarity << "%\n\n";

    report << "KMP Matches           : "
           << kmpMatches << "\n";

    report << "Rabin-Karp Matches    : "
           << rabinMatches << "\n\n";

    report << "--------------------------------------------\n";
    report << "MATCHING CONTENT\n";
    report << "--------------------------------------------\n\n";

    if (matches.empty())
    {
        report << "No significant matching chunks found.\n";
    }
    else
    {
        for (int i = 0; i < matches.size(); i++)
        {
            report << i + 1 << ". "
                   << matches[i] << "\n";
        }
    }

    report << "\n============================================\n";

    report.close();

    cout << "\nReport generated successfully!\n";
}


// ============================================================
// 10. MAIN PROGRAM
// ============================================================

int main()
{
    cout << "============================================\n";
    cout << "       PLAGIARISM DETECTION SYSTEM\n";
    cout << "============================================\n\n";

    cout << "Reading documents...\n";

    string doc1 = readFile("documents/document1.txt");
    string doc2 = readFile("documents/document2.txt");

    if (doc1.empty() || doc2.empty())
    {
        cout << "\nCould not read both documents.\n";
        return 1;
    }

    cout << "Documents loaded successfully.\n";

    // Preprocess
    doc1 = preprocess(doc1);
    doc2 = preprocess(doc2);

    cout << "Text preprocessing completed.\n";

    // Count total chunks
    vector<string> words = getWords(doc1);

    const int CHUNK_SIZE = 5;

    int totalChunks = words.size() / CHUNK_SIZE;

    int kmpMatches;
    int rabinMatches;

    // Find matching chunks
    vector<string> matches =
        findMatchingChunks(
            doc1,
            doc2,
            kmpMatches,
            rabinMatches
        );

    // Calculate similarity
    double similarity =
        calculateSimilarity(
            matches.size(),
            totalChunks
        );

    cout << "\n============================================\n";
    cout << "              ANALYSIS RESULT\n";
    cout << "============================================\n";

    cout << fixed << setprecision(2);

    cout << "Similarity : "
         << similarity << "%\n";

    cout << "KMP Matches : "
         << kmpMatches << "\n";

    cout << "Rabin-Karp Matches : "
         << rabinMatches << "\n";

    cout << "\nMatching Chunks:\n";

    if (matches.empty())
    {
        cout << "No matching chunks found.\n";
    }
    else
    {
        for (int i = 0; i < matches.size(); i++)
        {
            cout << i + 1 << ". "
                 << matches[i] << endl;
        }
    }

    // Generate report
    generateReport(
        matches,
        similarity,
        kmpMatches,
        rabinMatches
    );

    cout << "\n============================================\n";
    cout << "             ANALYSIS COMPLETE\n";
    cout << "============================================\n";

    return 0;
}
