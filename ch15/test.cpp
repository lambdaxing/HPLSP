#include <vector>
#include <iostream>
using std::vector;

void print(vector<int>& ns) {
    for(const auto& i : ns) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

void bubleSort(vector<int>& ns) {
    int len = ns.size();
    bool isSort;
    for(int i = 0; i < len - 1; i++) {
        isSort = true;
        for(int j = 0; j < len - 1 - i; j++) {
            if(ns[j] > ns[j+1]) {
                std::swap(ns[j], ns[j+1]);
                isSort = false;
            }
        }
        if(isSort)
            break;
    }
}

void selectionSort(vector<int>& ns) {
    int len = ns.size();
    for(int i = 0; i < len-1; i++) {
        int minIndex = i;
        for(int j = i+1; j < len; j++) {
            if(ns[j] < ns[minIndex])
                minIndex = j;
        }
        std::swap(ns[i], ns[minIndex]);
    }
}

void insertSort(vector<int>& ns) {
    int len = ns.size();
    for(int i = 1; i < len; i++) {
        int k = ns[i];
        int j = i - 1;
        while(j >= 0 && ns[j] > k) {
            ns[j+1] = ns[j];
            j--;
        }
        ns[j+1] = k;
    }
}

void quickSort(vector<int>& ns, int left, int right) {
    if(left >= right) return;
    int l = left, r = right, k = ns[l];
    while(l < r) {
        while(l < r && ns[r] >= k) {
            r--;
        }
        if(l < r) ns[l++] = ns[r];
        while(l < r && ns[l] <= k) {
            l++;
        }
        if(l < r) ns[r--] = ns[l];
    }
    ns[l] = k;
    quickSort(ns, left, l-1);
    quickSort(ns, l+1, right);
}

void shellSortCore(vector<int>& nums, int gap, int i) {
    int inserted = nums[i];
    int j;
    for(j = i - gap; j >= 0 && inserted < nums[j]; j -= gap) {
        nums[j + gap] = nums[j];
    }
    nums[j+gap] = inserted;
}

void shellSort(vector<int>& nums) {
    int len = nums.size();
    for(int gap = len/2; gap > 0; gap /= 2) {
        for(int i = gap; i < len; i++) {
            shellSortCore(nums, gap, i);
        }
    }
}

void mergeSort(vector<int>& data, vector<int>& temp, int left, int right) {
    if(left >= right) return;
    int mid = left + (right - left)/2;
    mergeSort(temp, data, left, mid);
    mergeSort(temp, data, mid+1, right);
    int index = left;
    int i,j;
    for(i = left, j = mid+1; i <= mid && j <= right;) {
        if(data[i] < data[j])
            temp[index++] = data[i++];
        else
            temp[index++] = data[j++];
    }
    while(i <= mid)
        temp[index++] = data[i++];
    while(j <= right)
        temp[index++] = data[j++];
}

void mergeSort(vector<int>& ns) {
    vector<int> temp(ns);
    mergeSort(temp, ns, 0, ns.size()-1);
}

int main() {
    vector<int> ns{1,3,5,3,2,6,8,3,1,0};
    print(ns);
    //quickSort(ns, 0, ns.size()-1);
    //bubleSort(ns);
    //selectionSort(ns);
    //insertSort(ns);
    //shellSort(ns);
    mergeSort(ns);
    print(ns);
}