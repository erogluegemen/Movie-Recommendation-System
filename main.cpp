#include <cmath>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

const int length_user = 35190;
const int length_item = 7335;
const int trainLineCount = 1123345;
const int topUserCount = 130;


class tmp_obj {
public:
    int user_id, item_id;
    float rating;

    void set_obj(int Userid, int Itemid, float Rating) {
        user_id = Userid;
        item_id = Itemid;
        rating = Rating;
    }

    tmp_obj() {
        user_id = 0;
        item_id = 0;
        rating = 0;
    };

};

// ---------- Heapify ----------
void heapify(tmp_obj array[], int n, int i)
{
    int largest = i;
    int left = 2 * i + 1; 
    int right = 2 * i + 2; 

    if (left < n && array[left].user_id > array[largest].user_id) {
        largest = left;
    }

    if (right < n && array[right].user_id  > array[largest].user_id) {
        largest = right;
    }

    if (largest != i) {
        swap(array[i], array[largest]);
        heapify(array, n, largest);
    }
}

void heap_sort(tmp_obj array[], int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(array, n, i);

    for (int i = n - 1; i >= 0; i--) {
        swap(array[0], array[i]);
        heapify(array, i, 0);
    }
}

template <typename T>
class storage {
public:
    T* array;
    int last_obj_index;

    explicit storage(int size) {
        array = new T[size];
        last_obj_index = 0;
    }


    void next_obj_index(int id) {
        if (last_obj_index != id) {
            array[last_obj_index].next_object_index = id;
            last_obj_index = id;
        }
    }


    void add_item_id_to_rating_list(int item_id, float rating) {
        next_obj_index(item_id);
        array[item_id].rate = rating;
    }


    void add_rating_to_user(int user_id, int item_id, float rating) {
        next_obj_index(user_id);
        array[user_id].add_rating(item_id, rating);
    }


    ~storage() {
        delete[] array;
    }
};

class rating_class {
public:
    float rate;
    int next_object_index;

    explicit rating_class(float rating = -1.0) {
        rate = rating;
        next_object_index = 0;
    }
};

class user {
public:
    int next_object_index;
    storage<rating_class> rating_array_class = storage<rating_class>(length_item);

    user() {
        next_object_index = 0;
    }

    void add_rating(int item_id, float rating) {
        rating_array_class.add_item_id_to_rating_list(item_id, rating);
    }
};

storage<user> user_list = storage<user>(length_user);
string file = "ID,Predicted\n";


class compare_obj {
public:
    double similarity;
    int common;
    double rating;

    compare_obj() {
        similarity = -1;
        common = -1;
        rating = -1;
    }


    void set_obj(double sim, int com, double rat) {
        similarity = sim;
        common = com;
        rating = rat;
    }
};


void min_heapify(compare_obj array[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1; 
    int right = 2 * i + 2; 

    if (left < n && array[left].similarity > array[largest].similarity)
        largest = left;

    if (right < n && array[right].similarity > array[largest].similarity)
        largest = right;

    if (largest != i) {
        swap(array[i], array[largest]);
        min_heapify(array, n, largest);
    }
}

void min_heap_sort(compare_obj array[], int n)
{

    for (int i = n / 2 - 1; i >= 0; i--)
        min_heapify(array, n, i);


    for (int i = n - 1; i >= 0; i--) {
        swap(array[0], array[i]);
        min_heapify(array, i, 0);
    }
}


struct similarityOutput {
    double similarity;
    int common;
};


struct similarityOutput  calculate_manhattan(int question_user_id, int current_user_id, int* item_ids, int id_array_size) {
    struct similarityOutput output { 2147483647, 0 };


    int common = 0;
    double number = 0;
    for (int i = 0; i < id_array_size; i++) {
        double c_rat = user_list.array[current_user_id].rating_array_class.array[item_ids[i]].rate;
        if (c_rat != -1) {
            common++;
            double q_rat = user_list.array[question_user_id].rating_array_class.array[item_ids[i]].rate;

            number += abs(c_rat - q_rat);
        }
    }
    if (common != 0) {
        output.similarity = number / (5 * common);
        output.common = common;
    }
    return output;
};


void set_next_id(int* user_id, int item_id) {
    int next_index = user_list.array[*user_id].next_object_index;

    if (next_index == 0) *user_id = -1;

    else if (next_index == item_id) {
        *user_id = next_index;
        set_next_id(user_id, item_id);
    }

    else    *user_id = next_index;
}

void predict(int qid, int user_id, int item_id) {
    int first = 0;
    int second = 0;
    int item_ids[1633];

    while (true) {
        if (user_list.array[user_id].rating_array_class.array[first].rate != -1) {
            item_ids[second] = first;
            second++;
        }
        if (user_list.array[user_id].rating_array_class.array[first].next_object_index == 0) break;
        else first = user_list.array[user_id].rating_array_class.array[first].next_object_index;
    }
    int id_arr_len = second;


    compare_obj* results = new compare_obj[length_user];

    first = 0;
    second = 0;

    while (first != -1) {
        double current_rating = user_list.array[first].rating_array_class.array[item_id].rate;
        if (current_rating != -1) {
            struct similarityOutput output = calculate_manhattan(user_id, first, item_ids, id_arr_len);

            results[second].set_obj(output.similarity, output.common, current_rating);
            second++;
        }
        set_next_id(&first, item_id);
    }

    min_heap_sort(results, second);


    long double result = 0;
    int total = 0;

    for (int i = 0; i < second; i++) {
        if (i >= topUserCount)break;
        result += results[i].rating * results[i].common * results[i].common;
        total += results[i].common * results[i].common;
    }

    result = result / total;

    file += to_string(qid);
    file += ",";
    file += to_string(result);
    file += "\n";

    delete[] results;
}

int main() {
    tmp_obj* tmp_array = new tmp_obj[trainLineCount];

    string line;
    string user_id;
    string item_id;
    string rate;
    ifstream train("TXT_TRAIN_FILE_PATH");

    int i = 0;

    getline(train, line);
    while (getline(train, line)) {
        stringstream strstream(line);
        getline(strstream, user_id, ',');
        getline(strstream, item_id, ',');
        getline(strstream, rate, ',');

        tmp_array[i].set_obj(stoi(user_id), stoi(item_id), stof(rate));
        i++;
    }
    train.close();

    heap_sort(tmp_array, i);

    for (int a = 0; a < i; a++) {
        user_list.add_rating_to_user(tmp_array[a].user_id, tmp_array[a].item_id, tmp_array[a].rating);
    }
    delete[] tmp_array;


    ifstream test("TXT_TEST_FILE_PATH");
    string qid;
    getline(test, line);
    while (getline(test, line)) {
        stringstream strstream(line);
        getline(strstream, qid, ',');
        getline(strstream, user_id, ',');
        getline(strstream, item_id, ',');

        predict(stoi(qid), stoi(user_id), stoi(item_id));

    }
    train.close();

    std::ofstream out_file("C:/submission.csv");

    out_file << file;
    out_file.close();

    std::cout << "Added successfully " << std::endl;

    return 0;
}