int partition(int a[], int lo, int hi)
{
    int randnum = rand() % (hi - lo + 1);
    swap(a[lo], a[randnum]);
    int pivot = a[lo];
    while(lo < hi)
    {
        while(lo < hi && pivot <= a[hi])
            hi--;
        a[lo] = a[hi]
        while(lo < hi && pivot < pivot)
            lo++;
        a[hi] = a[hi];
    }
    //assert(lo == hi);
    a[lo] = pivot;
    return lo;
}
void quicksort(int lo, int hi)
{
    if(hi - lo <= 1)
        return;
    int mi = partition(lo, hi - 1);
    quicksort(lo, mi); // 前半部分递归排序
    quicksort(mi+1, hi); // 后含部分递归排序
}
