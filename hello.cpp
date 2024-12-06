#include <iostream>
#include <fstream>   // For file handling
#include <vector>    // For std::vector
#include <string>    // For std::string
using namespace std;

// Define the Person structure
struct Person {
    string name;
    int age;
    char sex;
    float weight;
};

void saveToCSV(const vector<Person>& persons, const string& filename) {
    // Create and open a file in write mode
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return;
    }

    // Write the CSV header
    file << "Name,Age,Sex,Weight\n";

    // Write data for each person
    for (const auto& person : persons) {
        file << person.name << "," << person.age << "," << person.sex << "," << person.weight << "\n";
    }

    // Close the file
    file.close();

    cout << "Data saved to " << filename << endl;
}

int main() {
    // Example: Array of persons
    vector<Person> persons;

    int n;
    cout << "Enter number of persons: ";
    cin >> n;

    // Input details for each person
    for (int i = 0; i < n; ++i) {
        Person p;
        cin.ignore();  // Ignore newline from previous input
        cout << "Enter name for person " << i + 1 << ": ";
        getline(cin, p.name);
        cout << "Enter age: ";
        cin >> p.age;
        cout << "Enter sex (M/F): ";
        cin >> p.sex;
        cout << "Enter weight: ";
        cin >> p.weight;
        persons.push_back(p);
    }

    // Save the array to a CSV file
    saveToCSV(persons, "persons.csv");

    return 0;
}
