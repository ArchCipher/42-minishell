# include "minishell.h"

static void     ft_qsort_helper(t_sort sort, size_t low, size_t high);
static size_t   partition(t_sort sort, size_t low, size_t high);
static void     memswap(void *a, void *b, size_t size);

/*
nel = number of elements
width = size of each element
*/
void    ft_qsort(void *base, size_t nel, size_t width, int (*f)(const void *, const void *))
{
    t_sort sort;

    sort.base = base;
    sort.width = width;
    sort.comp = f;
    if (nel > 1)
        ft_qsort_helper(sort, 0, nel - 1);
}

static void    ft_qsort_helper(t_sort sort, size_t low, size_t high)
{
    size_t pi;

    if (low < high)
    {
        pi = partition(sort, low, high);
        if (pi > 0)
            ft_qsort_helper(sort, low, pi - 1);
        ft_qsort_helper(sort, pi + 1, high);
    }
}

/*
[ <= pivot (i) | > pivot (j) | unknown | pivot ]
*/

static size_t partition(t_sort sort, size_t low, size_t high)
{
    size_t i;
    size_t j;

    char *arr = (char *)sort.base;
    i = low;
    j = low;
    while(j < high)
    {
        if (sort.comp(arr + j * sort.width, arr + high * sort.width) <= 0)
        {
            memswap(arr + i * sort.width, arr + j * sort.width, sort.width);
            i++;
        }
        j++;
    }
    memswap(arr + i * sort.width, arr + high * sort.width, sort.width);
    return (i);
}

/*
malloc is used as VLA (tmp[size]) is not valid in 42
*/
static void    memswap(void *a, void *b, size_t size)
{
    unsigned char *tmp;

    tmp = malloc(size);
    if (!tmp)
        return ; // handle error (cleanup and exit)
    ft_memcpy(tmp, a, size);
    ft_memcpy(a, b, size);
    ft_memcpy(b, tmp, size);
    free(tmp);
}
