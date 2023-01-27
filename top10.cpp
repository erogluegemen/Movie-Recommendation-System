#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
using namespace std;

const int maximumItem = 7335;
const int maximumUser = 35190;



class mainfunc {
public:
    int count;
    int id;

    mainfunc() {
        count = 0;
        id = 0;
    };

    void sets(int Id)
    {
        id = Id;
        count++;
    }
};



void heapifyfunc(mainfunc arr[], int m, int i){
    int smallest = i; 
    int right = 2 * i + 2; 
    int left = 2 * i + 1; 

    if (left < m && arr[left].count < arr[smallest].count)
        smallest = left;

    if (right < m && arr[right].count < arr[smallest].count)
        smallest = right;

    
    if (smallest != i) {
        swap(arr[i], arr[smallest]);

        
        heapifyfunc(arr, m, smallest);
    }
}


void heapSortfunc(mainfunc arr[], int n){
    
    for (int i = n / 2 - 1; i >= 0; i--)
        heapifyfunc(arr, n, i);
    
    for (int i = n - 1; i >= 0; i--) {
        swap(arr[0], arr[i]);
        heapifyfunc(arr, i, 0);
    }
}

int main()
{

    string rating;
    string itemid;
    string line;
    string userid;

    int maxUser = 0;
    int maxItem = 0;
    int lines = 0;

    mainfunc* userRatingCounts = new mainfunc[maximumUser];
    mainfunc* movieRatingCounts = new mainfunc[maximumItem];

    ifstream train("TXT_TRAIN_FILE_PATH");
    getline(train, line);
    while (getline(train, line))
    {
        stringstream strstream(line);
        getline(strstream, userid, ',');
        getline(strstream, itemid, ',');
        getline(strstream, rating, ',');
        int user_id = stoi(userid);
        int item_id = stoi(itemid);

        if (item_id > maxItem)maxItem = item_id;
        if (user_id > maxUser)maxUser = user_id;

        movieRatingCounts[item_id].sets(item_id);
        userRatingCounts[user_id].sets(user_id);

        lines++;
    }
    train.close();

    heapSortfunc(movieRatingCounts, maximumItem);
    heapSortfunc(userRatingCounts, maximumUser);

    cout << "\nTop 10 Users: " << endl;
    for (int k = 0; k < maximumUser; k++) {
        std::cout <<k+1<< "-" << " User id = " << userRatingCounts[k].id << " -- count = " << userRatingCounts[k].count << std::endl;
        if (k >= 9)
            break;
    }

    cout << "\nTop 10 Items:" << endl;
    for (int k = 0; k < maximumItem; k++) {
        std::cout << k+1<< "-" << " Item id = " << movieRatingCounts[k].id << " -- count = " << movieRatingCounts[k].count << std::endl;
        if (k >= 9)
            break;
    }

    

    
    
    cout << "\n MaxUserId = " << maxUser << "\n MaxItemId = " << maxItem << "\n [MConverter.eu].txt lines = " << lines << endl;


    delete[] movieRatingCounts;
    delete[] userRatingCounts;

    return 0;
}
