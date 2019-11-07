#include<stdio.h>
#include<math.h>
#include <stdlib.h> 
#include<strings.h>
#include <limits.h>
#include <float.h>

//Hierarchical clustering fuss for nomenclature
#define NOT_USED  0 /* node is currently not used */
#define LEAF_NODE 1 /* node contains a leaf node */
#define A_MERGER  2 /* node contains a merged pair of root clusters */
#define MAX_LABEL_LEN 16

#define AVERAGE_LINKAGE  'a' /* choose average distance */
#define CENTROID_LINKAGE 't' /* choose distance between cluster centroids */
#define COMPLETE_LINKAGE 'c' /* choose maximum distance */
#define SINGLE_LINKAGE   's' /* choose minimum distance */

#define alloc_mem(N, T) (T *) calloc(N, sizeof(T))
#define alloc_fail(M) fprintf(stderr,                                   \
                              "Failed to allocate memory for %s.\n", M)
#define read_fail(M) fprintf(stderr, "Failed to read %s from file.\n", M)
#define invalid_node(I) fprintf(stderr,                                 \
                                "Invalid cluster node at index %d.\n", I)
//functions for Hierarchical Clustering
typedef struct cluster_s cluster_t;
typedef struct cluster_node_s cluster_node_t;
typedef struct neighbour_s neighbour_t;
typedef struct item_s item_t;

float (*distance_fptr)(float **, const int *, const int *, int, int);

typedef struct coord_s {
        float x, y;
} coord_t;

struct cluster_s {
        int num_items; /* number of items that was clustered */
        int num_clusters; /* current number of root clusters */
        int num_nodes; /* number of leaf and merged clusters */
        cluster_node_t *nodes; /* leaf and merged clusters */
        float **distances; /* distance between leaves */
};

struct cluster_node_s {
        int type; /* type of the cluster node */
        int is_root; /* true if cluster hasn't merged with another */
        int height; /* height of node from the bottom */
        coord_t centroid; /* centroid of this cluster */
        char *label; /* label of a leaf node */
        int *merged; /* indexes of root clusters merged */
        int num_items; /* number of leaf nodes inside new cluster */
        int *items; /* array of leaf nodes indices inside merged clusters */
        neighbour_t *neighbours; /* sorted linked list of distances to roots */
};

struct neighbour_s {
        int target; /* the index of cluster node representing neighbour */
        float distance; /* distance between the nodes */
        neighbour_t *next, *prev; /* linked list entries */
};

struct item_s {
        coord_t coord; /* coordinate of the input data point */
        char label[MAX_LABEL_LEN]; /* label of the input data point */
};

float euclidean_distance(const coord_t *a, const coord_t *b)
{
        return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

void fill_euclidean_distances(float **matrix, int num_items,
                              const item_t items[])
{
        for (int i = 0; i < num_items; ++i)
                for (int j = 0; j < num_items; ++j) {
                        matrix[i][j] =
                                euclidean_distance(&(items[i].coord),
                                                   &(items[j].coord));
                        matrix[j][i] = matrix[i][j];
                }
}

float **generate_distance_matrix(int num_items, const item_t items[])
{
        float **matrix = alloc_mem(num_items, float *);
        if (matrix) {
                for (int i = 0; i < num_items; ++i) {
                        matrix[i] = alloc_mem(num_items, float);
                        if (!matrix[i]) {
                                alloc_fail("distance matrix row");
                                num_items = i;
                                for (i = 0; i < num_items; ++i)
                                        free(matrix[i]);
                                free(matrix);
                                matrix = NULL;
                                break;
                        }
                }
                if (matrix)
                        fill_euclidean_distances(matrix, num_items, items);
        } else
                alloc_fail("distance matrix");
        return matrix;
}

float single_linkage(float **distances, const int a[],
                     const int b[], int m, int n)
{
        float min = FLT_MAX, d;
        for (int i = 0; i < m; ++i)
                for (int j = 0; j < n; ++j) {
                        d = distances[a[i]][b[j]];
                        if (d < min)
                                min = d;
                }
        return min;
}

float complete_linkage(float **distances, const int a[],
                       const int b[], int m, int n)
{
        float d, max = 0.0 /* assuming distances are positive */;
        for (int i = 0; i < m; ++i)
                for (int j = 0; j < n; ++j) {
                        d = distances[a[i]][b[j]];
                        if (d > max)
                                max = d;
                }
        return max;
}

float average_linkage(float **distances, const int a[],
                      const int b[], int m, int n)
{
        float total = 0.0;
        for (int i = 0; i < m; ++i)
                for (int j = 0; j < n; ++j)
                        total += distances[a[i]][b[j]];
        return total / (m * n);
}

float centroid_linkage(float **distances, const int a[],
                       const int b[], int m, int n)
{
        return 0; /* empty function */
}

float get_distance(cluster_t *cluster, int index, int target)
{
        /* if both are leaves, just use the distances matrix */
        if (index < cluster->num_items && target < cluster->num_items)
                return cluster->distances[index][target];
        else {
                cluster_node_t *a = &(cluster->nodes[index]);
                cluster_node_t *b = &(cluster->nodes[target]);
                if (distance_fptr == centroid_linkage)
                        return euclidean_distance(&(a->centroid),
                                                  &(b->centroid));
                else return distance_fptr(cluster->distances,
                                          a->items, b->items,
                                          a->num_items, b->num_items);
        }
}

void free_neighbours(neighbour_t *node)
{
        neighbour_t *t;
        while (node) {
                t = node->next;
                free(node);
                node = t;
        }
}

void free_cluster_nodes(cluster_t *cluster)
{
        for (int i = 0; i < cluster->num_nodes; ++i) {
                cluster_node_t *node = &(cluster->nodes[i]);
                if (node->label)
                        free(node->label);
                if (node->merged)
                        free(node->merged);
                if (node->items)
                        free(node->items);
                if (node->neighbours)
                        free_neighbours(node->neighbours);
        }
        free(cluster->nodes);
}

void free_cluster(cluster_t * cluster)
{
        if (cluster) {
                if (cluster->nodes)
                        free_cluster_nodes(cluster);
                if (cluster->distances) {
                        for (int i = 0; i < cluster->num_items; ++i)
                                free(cluster->distances[i]);
                        free(cluster->distances);
                }
                free(cluster);
        }
}

void insert_before(neighbour_t *current, neighbour_t *neighbours,
                   cluster_node_t *node)
{
        neighbours->next = current;
        if (current->prev) {
                current->prev->next = neighbours;
                neighbours->prev = current->prev;
        } else
                node->neighbours = neighbours;
        current->prev = neighbours;
}

void insert_after(neighbour_t *current, neighbour_t *neighbours)
{
        neighbours->prev = current;
        current->next = neighbours;
}

void insert_sorted(cluster_node_t *node, neighbour_t *neighbours)
{
        neighbour_t *temp = node->neighbours;
        while (temp->next) {
                if (temp->distance >= neighbours->distance) {
                        insert_before(temp, neighbours, node);
                        return;
                }
                temp = temp->next;
        }
        if (neighbours->distance < temp->distance)
                insert_before(temp, neighbours, node);
        else
                insert_after(temp, neighbours);
}


neighbour_t *add_neighbour(cluster_t *cluster, int index, int target)
{
        neighbour_t *neighbour = alloc_mem(1, neighbour_t);
        if (neighbour) {
                neighbour->target = target;
                neighbour->distance = get_distance(cluster, index, target);
                cluster_node_t *node = &(cluster->nodes[index]);
                if (node->neighbours)
                        insert_sorted(node, neighbour);
                else
                        node->neighbours = neighbour;
        } else
                alloc_fail("neighbour node");
        return neighbour;
}

cluster_t *update_neighbours(cluster_t *cluster, int index)
{
        cluster_node_t *node = &(cluster->nodes[index]);
        if (node->type == NOT_USED) {
                invalid_node(index);
                cluster = NULL;
        } else {
                int root_clusters_seen = 1, target = index;
                while (root_clusters_seen < cluster->num_clusters) {
                        cluster_node_t *temp = &(cluster->nodes[--target]);
                        if (temp->type == NOT_USED) {
                                invalid_node(index);
                                cluster = NULL;
                                break;
                        }
                        if (temp->is_root) {
                                ++root_clusters_seen;
                                add_neighbour(cluster, index, target);
                        }
                }
        }
        return cluster;
}

#define init_leaf(cluster, node, item, len)             \
        do {                                            \
                strncpy(node->label, item->label, len); \
                node->centroid = item->coord;           \
                node->type = LEAF_NODE;                 \
                node->is_root = 1;                      \
                node->height = 0;                       \
                node->num_items = 1;                    \
                node->items[0] = cluster->num_nodes++;  \
        } while (0)                                     \

cluster_node_t *add_leaf(cluster_t *cluster, const item_t *item)
{
        cluster_node_t *leaf = &(cluster->nodes[cluster->num_nodes]);
        int len = strlen(item->label) + 1;
        leaf->label = alloc_mem(len, char);
        if (leaf->label) {
                leaf->items = alloc_mem(1, int);
                if (leaf->items) {
                        init_leaf(cluster, leaf, item, len);
                        cluster->num_clusters++;
                } else {
                        alloc_fail("node items");
                        free(leaf->label);
                        leaf = NULL;
                }
        } else {
                alloc_fail("node label");
                leaf = NULL;
        }
        return leaf;
}

#undef init_leaf

cluster_t *add_leaves(cluster_t *cluster, item_t *items)
{
        for (int i = 0; i < cluster->num_items; ++i) {
                if (add_leaf(cluster, &items[i]))
                        update_neighbours(cluster, i);
                else {
                        cluster = NULL;
                        break;
                }
        }
        return cluster;
}

void print_cluster_items(cluster_t *cluster, int index)
{
        cluster_node_t *node = &(cluster->nodes[index]);
        fprintf(stdout, "Items: ");
        if (node->num_items > 0) {
                fprintf(stdout, "%s", cluster->nodes[node->items[0]].label);
                for (int i = 1; i < node->num_items; ++i)
                        fprintf(stdout, ", %s",
                                cluster->nodes[node->items[i]].label);
        }
        fprintf(stdout, "\n");
}

void print_cluster_node(cluster_t *cluster, int index)
{
        cluster_node_t *node = &(cluster->nodes[index]);
        fprintf(stdout, "Node %d - height: %d, centroid: (%5.3f, %5.3f)\n",
                index, node->height, node->centroid.x, node->centroid.y);
        if (node->label)
                fprintf(stdout, "\tLeaf: %s\n\t", node->label);
        else
                fprintf(stdout, "\tMerged: %d, %d\n\t",
                        node->merged[0], node->merged[1]);
        print_cluster_items(cluster, index);
        fprintf(stdout, "\tNeighbours: ");
        neighbour_t *t = node->neighbours;
        while (t) {
                fprintf(stdout, "\n\t\t%2d: %5.3f", t->target, t->distance);
                t = t->next;
        }
        fprintf(stdout, "\n");
}

void merge_items(cluster_t *cluster, cluster_node_t *node,
                 cluster_node_t **to_merge)
{
        node->type = A_MERGER;
        node->is_root = 1;
        node->height = -1;

        /* copy leaf indexes from merged clusters */
        int k = 0, idx;
        coord_t centroid = { .x = 0.0, .y = 0.0 };
        for (int i = 0; i < 2; ++i) {
                cluster_node_t *t = to_merge[i];
                t->is_root = 0; /* no longer root: merged */
                if (node->height == -1 ||
                    node->height < t->height)
                        node->height = t->height;
                for (int j = 0; j < t->num_items; ++j) {
                        idx = t->items[j];
                        node->items[k++] = idx;
                }
                centroid.x += t->num_items * t->centroid.x;
                centroid.y += t->num_items * t->centroid.y;
        }
        /* calculate centroid */
        node->centroid.x = centroid.x / k;
        node->centroid.y = centroid.y / k;
        node->height++;
}

#define merge_to_one(cluster, to_merge, node, node_idx)         \
        do {                                                    \
                node->num_items = to_merge[0]->num_items +      \
                        to_merge[1]->num_items;                 \
                node->items = alloc_mem(node->num_items, int);  \
                if (node->items) {                              \
                        merge_items(cluster, node, to_merge);   \
                        cluster->num_nodes++;                   \
                        cluster->num_clusters--;                \
                        update_neighbours(cluster, node_idx);   \
                } else {                                        \
                        alloc_fail("array of merged items");    \
                        free(node->merged);                     \
                        node = NULL;                            \
                }                                               \
        } while(0)                                              \

cluster_node_t *merge(cluster_t *cluster, int first, int second)
{
        int new_idx = cluster->num_nodes;
        cluster_node_t *node = &(cluster->nodes[new_idx]);
        node->merged = alloc_mem(2, int);
        if (node->merged) {
                cluster_node_t *to_merge[2] = {
                        &(cluster->nodes[first]),
                        &(cluster->nodes[second])
                };
                node->merged[0] = first;
                node->merged[1] = second;
                merge_to_one(cluster, to_merge, node, new_idx);
        } else {
                alloc_fail("array of merged nodes");
                node = NULL;
        }
        return node;
}

#undef merge_to_one

void find_best_distance_neighbour(cluster_node_t *nodes,
                                  int node_idx,
                                  neighbour_t *neighbour,
                                  float *best_distance,
                                  int *first, int *second)
{
        while (neighbour) {
                if (nodes[neighbour->target].is_root) {
                        if (*first == -1 ||
                            neighbour->distance < *best_distance) {
                                *first = node_idx;
                                *second = neighbour->target;
                                *best_distance = neighbour->distance;
                        }
                        break;
                }
                neighbour = neighbour->next;
        }
}


int find_clusters_to_merge(cluster_t *cluster, int *first, int *second)
{
        float best_distance = 0.0;
        int root_clusters_seen = 0;
        int j = cluster->num_nodes; /* traverse hierarchy top-down */
        *first = -1;
        while (root_clusters_seen < cluster->num_clusters) {
                cluster_node_t *node = &(cluster->nodes[--j]);
                if (node->type == NOT_USED || !node->is_root)
                        continue;
                ++root_clusters_seen;
                find_best_distance_neighbour(cluster->nodes, j,
                                             node->neighbours,
                                             &best_distance,
                                             first, second);
        }
        return *first;
}

cluster_t *merge_clusters(cluster_t *cluster)
{
        int first, second;
        while (cluster->num_clusters > 1) {
                if (find_clusters_to_merge(cluster, &first, &second) != -1)
                        merge(cluster, first, second);
        }
        return cluster;
}

#define init_cluster(cluster, num_items, items)                         \
        do {                                                            \
                cluster->distances =                                    \
                        generate_distance_matrix(num_items, items);     \
                if (!cluster->distances)                                \
                        goto cleanup;                                   \
                cluster->num_items = num_items;                         \
                cluster->num_nodes = 0;                                 \
                cluster->num_clusters = 0;                              \
                if (add_leaves(cluster, items))                         \
                        merge_clusters(cluster);                        \
                else                                                    \
                        goto cleanup;                                   \
        } while (0)                                                     \

cluster_t *agglomerate(int num_items, item_t *items)
{
        cluster_t *cluster = alloc_mem(1, cluster_t);
        if (cluster) {
                cluster->nodes = alloc_mem(2 * num_items - 1, cluster_node_t);
                if (cluster->nodes)
                        init_cluster(cluster, num_items, items);
                else {
                        alloc_fail("cluster nodes");
                        goto cleanup;
                }
        } else
                alloc_fail("cluster");
        goto done;

cleanup:
        free_cluster(cluster);
        cluster = NULL;

done:
        return cluster;
}

#undef init_cluster

int print_root_children(cluster_t *cluster, int i, int nodes_to_discard)
{
        cluster_node_t *node = &(cluster->nodes[i]);
        int roots_found = 0;
        if (node->type == A_MERGER) {
                for (int j = 0; j < 2; ++j) {
                        int t = node->merged[j];
                        if (t < nodes_to_discard) {
                                print_cluster_items(cluster, t);
                                ++roots_found;
                        }
                }
        }
        return roots_found;
}

void get_k_clusters(cluster_t *cluster, int k)
{
        if (k < 1)
                return;
        if (k > cluster->num_items)
                k = cluster->num_items;

        int i = cluster->num_nodes - 1;
        int roots_found = 0;
        int nodes_to_discard = cluster->num_nodes - k + 1;
        while (k) {
                if (i < nodes_to_discard) {
                        print_cluster_items(cluster, i);
                        roots_found = 1;
                } else
                        roots_found = print_root_children(cluster, i,
                                                          nodes_to_discard);
                k -= roots_found;
                --i;
        }
}

void print_cluster(cluster_t *cluster)
{
        for (int i = 0; i < cluster->num_nodes; ++i)
                print_cluster_node(cluster, i);
}

int read_items(int count, item_t *items, FILE *f)
{
        for (int i = 0; i < count; ++i) {
                item_t *t = &(items[i]);
                if (fscanf(f, "%[^|]| %10f %10f\n",
                           t->label, &(t->coord.x),
                           &(t->coord.y)))
                        continue;
                read_fail("item line");
                return i;
        }
        return count;
}

int read_items_from_file(item_t **items, FILE *f)
{
        int count, r;
        r = fscanf(f, "%5d\n", &count);
        if (r == 0) {
                read_fail("number of lines");
                return 0;
        }
        if (count) {
                *items = alloc_mem(count, item_t);
                if (*items) {
                        if (read_items(count, *items, f) != count)
                                free(items);
                } else
                        alloc_fail("items array");
        }
        return count;
}

void set_linkage(char linkage_type)
{
        switch (linkage_type) {
        case AVERAGE_LINKAGE:
                distance_fptr = average_linkage;
                break;
        case COMPLETE_LINKAGE:
                distance_fptr = complete_linkage;
                break;
        case CENTROID_LINKAGE:
                distance_fptr = centroid_linkage;
                break;
        case SINGLE_LINKAGE:
        default: distance_fptr = single_linkage;
        }
}

int process_input(item_t **items, const char *fname)
{
        int count = 0;
        FILE *f = fopen(fname, "r");
        if (f) {
                count = read_items_from_file(items, f);
                fclose(f);
        } else
                fprintf(stderr, "Failed to open input file %s.\n", fname);
        return count;
}



//till here hierarchical 

//for DBSCAN 
#define UNCLASSIFIED -1
#define NOISE -2

#define CORE_POINT 1
#define NOT_CORE_POINT 0

#define SUCCESS 0
#define FAILURE -3
//till here for DBSCAN


//easy for nomenclature for DBSCAN
typedef struct point_s point_t;
struct point_s {
    double x, y, z;
    int cluster_id;
};

typedef struct node_s node_t;
struct node_s {
    unsigned int index;
    node_t *next;
};

typedef struct epsilon_neighbours_s epsilon_neighbours_t;
struct epsilon_neighbours_s {
    unsigned int num_members;
    node_t *head, *tail;
};

//till here easy for nomenclature for DBSCAN
// functions for DBSCAN 
node_t *create_node(unsigned int index);
int append_at_end(
     unsigned int index,
     epsilon_neighbours_t *en);
epsilon_neighbours_t *get_epsilon_neighbours(
    unsigned int index,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    double (*dist)(point_t *a, point_t *b));
void print_epsilon_neighbours(
    point_t *points,
    epsilon_neighbours_t *en);
void destroy_epsilon_neighbours(epsilon_neighbours_t *en);
void dbscan(
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b));
int expand(
    unsigned int index,
    unsigned int cluster_id,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b));
int spread(
    unsigned int index,
    epsilon_neighbours_t *seeds,
    unsigned int cluster_id,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b));
double euclidean_dist(point_t *a, point_t *b);
double adjacent_intensity_dist(point_t *a, point_t *b);
unsigned int parse_input(
    FILE *file,
    point_t **points,
    double *epsilon,
    unsigned int *minpts);
void print_points(
    point_t *points,
    unsigned int num_points);

node_t *create_node(unsigned int index)
{
    node_t *n = (node_t *) calloc(1, sizeof(node_t));
    if (n == NULL)
        perror("Failed to allocate node.");
    else {
        n->index = index;
        n->next = NULL;
    }
    return n;
}

int append_at_end(
     unsigned int index,
     epsilon_neighbours_t *en)
{
    node_t *n = create_node(index);
    if (n == NULL) {
        free(en);
        return FAILURE;
    }
    if (en->head == NULL) {
        en->head = n;
        en->tail = n;
    } else {
        en->tail->next = n;
        en->tail = n;
    }
    ++(en->num_members);
    return SUCCESS;
}

epsilon_neighbours_t *get_epsilon_neighbours(
    unsigned int index,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    double (*dist)(point_t *a, point_t *b))
{
    epsilon_neighbours_t *en = (epsilon_neighbours_t *)
        calloc(1, sizeof(epsilon_neighbours_t));
    if (en == NULL) {
        perror("Failed to allocate epsilon neighbours.");
        return en;
    }
    for (int i = 0; i < num_points; ++i) {
        if (i == index)
            continue;
        if (dist(&points[index], &points[i]) > epsilon)
            continue;
        else {
            if (append_at_end(i, en) == FAILURE) {
                destroy_epsilon_neighbours(en);
                en = NULL;
                break;
            }
        }
    }
    return en;
}

void print_epsilon_neighbours(
    point_t *points,
    epsilon_neighbours_t *en)
{
    if (en) {
        node_t *h = en->head;
        while (h) {
            printf("(%lfm, %lf, %lf)\n",
                   points[h->index].x,
                   points[h->index].y,
                   points[h->index].z);
            h = h->next;
        }
    }
}

void destroy_epsilon_neighbours(epsilon_neighbours_t *en)
{
    if (en) {
        node_t *t, *h = en->head;
        while (h) {
            t = h->next;
            free(h);
            h = t;
        }
        free(en);
    }
}

void dbscan(
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b))
{
    unsigned int i, cluster_id = 0;
    for (i = 0; i < num_points; ++i) {
        if (points[i].cluster_id == UNCLASSIFIED) {
            if (expand(i, cluster_id, points,
                       num_points, epsilon, minpts,
                       dist) == CORE_POINT)
                ++cluster_id;
        }
    }
}

int expand(
    unsigned int index,
    unsigned int cluster_id,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b))
{
    int return_value = NOT_CORE_POINT;
    epsilon_neighbours_t *seeds =
        get_epsilon_neighbours(index, points,
                               num_points, epsilon,
                               dist);
    if (seeds == NULL)
        return FAILURE;

    if (seeds->num_members < minpts)
        points[index].cluster_id = NOISE;
    else {
        points[index].cluster_id = cluster_id;
        node_t *h = seeds->head;
        while (h) {
            points[h->index].cluster_id = cluster_id;
            h = h->next;
        }

        h = seeds->head;
        while (h) {
            spread(h->index, seeds, cluster_id, points,
                   num_points, epsilon, minpts, dist);
            h = h->next;
        }

        return_value = CORE_POINT;
    }
    destroy_epsilon_neighbours(seeds);
    return return_value;
}

int spread(
    unsigned int index,
    epsilon_neighbours_t *seeds,
    unsigned int cluster_id,
    point_t *points,
    unsigned int num_points,
    double epsilon,
    unsigned int minpts,
    double (*dist)(point_t *a, point_t *b))
{
    epsilon_neighbours_t *spread =
        get_epsilon_neighbours(index, points,
                       num_points, epsilon,
                       dist);
    if (spread == NULL)
        return FAILURE;
    if (spread->num_members >= minpts) {
        node_t *n = spread->head;
        point_t *d;
        while (n) {
            d = &points[n->index];
            if (d->cluster_id == NOISE ||
                d->cluster_id == UNCLASSIFIED) {
                if (d->cluster_id == UNCLASSIFIED) {
                    if (append_at_end(n->index, seeds)
                        == FAILURE) {
                        destroy_epsilon_neighbours(spread);
                        return FAILURE;
                    }
                }
                d->cluster_id = cluster_id;
            }
            n = n->next;
        }
    }

    destroy_epsilon_neighbours(spread);
    return SUCCESS;
}

double euclidean_dist(point_t *a, point_t *b)
{
    return sqrt(pow(a->x - b->x, 2) +
            pow(a->y - b->y, 2) +
            pow(a->z - b->z, 2));
}

unsigned int parse_input(
    FILE *file,
    point_t **points,
    double *epsilon,
    unsigned int *minpts)
{
    unsigned int num_points, i = 0;
    fscanf(file, "%lf %u %u\n",
           epsilon, minpts, &num_points);
    point_t *p = (point_t *)
        calloc(num_points, sizeof(point_t));
    if (p == NULL) {
        perror("Failed to allocate points array");
        return 0;
    }
    while (i < num_points) {
          fscanf(file, "%lf %lf %lf\n",
                 &(p[i].x), &(p[i].y), &(p[i].z));
          p[i].cluster_id = UNCLASSIFIED;
          ++i;
    }
    *points = p;
    return num_points;
}

void print_points(
    point_t *points,
    unsigned int num_points)
{
    unsigned int i = 0;
    printf("Number of points: %u\n"
        " x     y     z     cluster_id\n"
        "-----------------------------\n"
        , num_points);
    while (i < num_points) {
          printf("%5.2lf %5.2lf %5.2lf: %d\n",
                 points[i].x,
                 points[i].y, points[i].z,
                 points[i].cluster_id);
          ++i;
    }
}

// till here functions for DBSCAN 



int main(){
//variables for Kmeans
int i,i1,i2,i3,i4, centroid1, centroid2,centroid3,temp1, temp2,temp3,prev_centroid1,prev_centroid2,prev_centroid3,f1,f2;
FILE *fptr1;
FILE *fptr2;
int data_set[10];
char product[20];
int cluster1[10], cluster2[10],cluster3[10];
int choice,choose_algo;
while(1){
printf("\n");
printf("\n");
printf("********************************************Welcome to Clustering Application, Which Algorithm would you like to work with?*********************************************");
printf("\n");
printf("\n");
printf("Enter 1 : for Kmeans Clustering\nEnter 2 : for Hierarchical Clustering\nEnter 3 : for DBSCAN\nEnter 0 : to exit application");	
printf("\n");
scanf("%d",&choose_algo);

//variables for hierarchical
int k ;
char link; 
item_t *items = NULL;
int num_items;




// variables for DBSCAN
double epsilon;
point_t *points;
   
unsigned int minpts;
FILE *dbfile;
    
unsigned int num_points;



//choose_algo switch case
switch(choose_algo){


case 1:

  while(1){

        printf("***************************************Welcome to Kmeans clustering Application, How many clusters would you like to work with?********************************");
        printf("\n");
        printf("\n");
        printf("Enter 2 : for 2 Clusters\nEnter 3 : for 3 Cluster\nEnter 0 : to exit kmeans clustering application and go to main menue ");	
        printf("\n");

        scanf("%d",&choice);

        //K means switch case
        switch(choice){

            case 2:

              /*int i,i1,i2, centroid1, centroid2,temp1, temp2,i3,prev_centroid1,prev_centroid2,f1,f2;
              int data_set[10];
              char product[20];
              int cluster1[10], cluster2[10];*/

              printf("\n");
              printf("**************************************************************Welcome to Kmeans Clustering Algorithm**************************************************************");
              printf("\n");
              //User is asked for the name of the product
              printf("\nEnter the Name of the product(20 Characters only)\n");
              scanf("%s",&product);
               //gets(product);
              printf("\n");
              printf("Sales Strategies for selling of %s is to divide the customer base into 2 groups on basis of no of products bought and taget market the two groups seperately",product);
               
			   /*
	           //user is asked for the number of products bought by 10 customers
               for(i=0;i<10;i++)
              {
               printf("\nEnter number of %s bought by customer %d \n", &product, i+1);
       	       scanf("%d",&data_set[i]);
			  }*/

              //writing to a file
              printf("\n");
              printf("\n");
              //FILE *fptr1;
              fptr1 = fopen("customer_data.txt","w");
     
              if(fptr1 == NULL)
              {
              printf("Error!"); 
              }
     
              for(f1=0;f1<10;f1++){
              printf("Enter Number of %s bought by customer %d :",product,f1+1);
              scanf("%d",&data_set[f1]);
     
              fprintf(fptr1,"%d",data_set[f1]); 
              fprintf(fptr1," "," "); 
              }
    
              fclose(fptr1); 
              printf("\n");  
              printf("Customer data entered into the file");
              printf("\n");
              //till here writing
         
              //reading from the file
              //FILE *fptr2;
              fptr2 = fopen("customer_data.txt", "r");

              //read file into array
   
              if (fptr2 == NULL){
              printf("Error Reading File\n");
       
              }

              for (f2 = 0; f2 < 10; f2++){
              fscanf(fptr2, "%d,", &data_set[f2] );
              }
    
	          printf("\n");         
    
	          for (f2 = 0; f2< 10; f2++){
              printf("Number of %s bought by customer %d is : %d\n\n",product, f2+1 , data_set[f2]);
              }

              fclose(fptr2);

 
              printf("Customer data fetched from the file");
              printf("\n");
              //till here reading
 
 
              //Ask user for intital centroids
              printf("\n Enter initial centroid1\t");
              scanf("%d",&centroid1);
              printf("\n Enter initial centroid2\t");
              scanf("%d",&centroid2);
  
              do{
              i=i2=i3=0;
              //saving previous mean so we can update centroid later

              prev_centroid1= centroid1;
              prev_centroid2= centroid2;

              //decision made as to which cluster this data will go to
              for(i1=0;i1<10;i1++)
              {
              //calculating distance to means
              temp1=data_set[i1]-centroid1;
              if(temp1<0){temp1=-temp1;}
         
              temp2=data_set[i1]-centroid2;
              if(temp2<0){temp2=-temp2;}

              if(temp1<temp2)
              {
              //near to first mean
              cluster1[i2]=data_set[i1];
              i2++;
              }
              else
              {
               //near to second mean

               cluster2[i3]=data_set[i1];
               i3++;
              }

              }

               //Updating centroids by calculating mean 
               temp2=0;
   
               for(temp1=0;temp1<i2;temp1++)
               {
               temp2=temp2+cluster1[temp1];
               }
                centroid1=temp2/i2;

                temp2=0;
                for(temp1=0;temp1<i3;temp1++)
                {  
                temp2=temp2+cluster2[temp1];
                }
                centroid2=temp2/i3;

                //Printing clusters 
  
                printf("\n");
                printf("Type 1 customers in cluster 1\t");
                printf("\n");

                for(temp1=0;temp1<i2;temp1++)
                {   
				printf("\t");
                printf("%d",cluster1[temp1]);
                }  
                printf("\tCentroid 1=%d",centroid1);


                printf("\n");
                printf("\n");

                printf("Type 2 customers in cluster 2\t");


                printf("\n");


                for(temp1=0;temp1<i3;temp1++)
                {    printf("\t");
                printf("%d",cluster2[temp1]);
                }   
                printf("\tcentroid 2=%d",centroid2);
                printf("\n");
                printf("\n");

                // Loop terminates after the next calculated mean is equal to the previous mean
                }

                while(centroid1!=prev_centroid1&&centroid2!=prev_centroid2);
                break;

         case 3:

               /*int i,i1,i2,i3,i4, centroid1, centroid2,centroid3,temp1, temp2,temp3,prev_centroid1,prev_centroid2,prev_centroid3,f1,f2;
               int data_set[10];
               char product[20];
               int cluster1[10], cluster2[10],cluster3[10];*/

               printf("\n");
               printf("**************************************************************Welcome to Kmeans Clustering Algorithm**************************************************************");

               //User is asked for the name of the product
               printf("\nEnter the Name of the product(20 Characters only)\n");
               scanf("%s",&product);
               printf("\n"); 
               printf("\n");
               printf("Sales Strategies for selling of %s is to divide the customer base into 3 groups on basis of no of products bought and taget market the three groups seperately",product);
               printf("\n");
               printf("\n");
               /*  Initial version of the program collected data from the user , calculated and discarded values.
	           // user is asked for the number of products bought by 10 customers

               for(i=0;i<10;i++) 
               {

		       printf("\nEnter number of %s bought by customer %d \n", &product, i+1);
	           scanf("%d",&data_set[i]);

               }*/

               //writing to a file
               //writing mode can be changed to append mode when software given to the client
               printf("\n");
               printf("\n");
               //  FILE *fptr1;
               fptr1 = fopen("customer_data.txt","w");
     
               if(fptr1 == NULL)
               {
                printf("Error!"); 
               }
     
               for(f1=0;f1<10;f1++){
               printf("Enter Number of %s bought by customer %d :",product,f1+1);
               scanf("%d",&data_set[f1]);
     
               fprintf(fptr1,"%d",data_set[f1]); 
               fprintf(fptr1," "," "); 
                } 
    
	           fclose(fptr1); 
               printf("\n");
               printf("Customer data entered into the file");
               printf("\n");
               //till here writing

               //reading from the file
               // FILE *fptr2;
               fptr2 = fopen("customer_data.txt", "r");

               //read file into array
   
               if (fptr2 == NULL){
               printf("Error Reading File\n");
       
               }

               for (f2 = 0; f2 < 10; f2++){
               fscanf(fptr2, "%d,", &data_set[f2] );
               }
    
	           printf("\n");         
    
	           for (f2 = 0; f2< 10; f2++){
               printf("Number of %s bought by customer %d is : %d\n\n",product,f2+1 , data_set[f2]);
               }

               fclose(fptr2);
               printf("\n");  
               printf("Customer data fetched from the file");
               printf("\n");
               //instead of collecting data from user, program can be cutomised to fetch from files created by client.
               //till here reading
 
 
               //Ask user for intital centroids

               printf("\n Enter initial centroid1\t");
               scanf("%d",&centroid1);
               printf("\n Enter initial centroid2\t");
               scanf("%d",&centroid2);
               printf("\n Enter initial centroid3\t");
               scanf("%d",&centroid3);
  
               do{
               i=i2=i3=i4=0;

               //saving previous mean so we can update centroid later

               prev_centroid1= centroid1;
               prev_centroid2= centroid2;
               prev_centroid3= centroid3;


               //decision made as to which cluster this data will go to
               for(i1=0;i1<10;i1++)
               {  
        
               temp1=data_set[i1]-centroid1;
               if(temp1<0){
		       temp1=-temp1;
	 	       }
        
               temp2=data_set[i1]-centroid2;
               if(temp2<0){
		       temp2=-temp2;
		       }

               temp3=data_set[i1]-centroid3;
               if(temp3<0){
		       temp3=-temp3;
		       }
       
	           //calculating distance to means
	           if(temp1<=temp2&&temp1<=temp3)
               {
               //near to first mean
               cluster1[i2]=data_set[i1];
               i2++;
               }
               else if(temp2<temp1&&temp2<=temp3)
               {
                //near to second mean

               cluster2[i3]=data_set[i1];
                i3++;
               }
               else 
               {
                //near to third mean

               cluster3[i4]=data_set[i1];
               i4++;
                }


                }

  
  
               //Updating centroids by calculating mean 
     
	           temp2=0;
              for(temp1=0;temp1<i2;temp1++)
              {
              temp2=temp2+cluster1[temp1];
              }
              centroid1=temp2/i2;

              temp2=0;
              for(temp1=0;temp1<i3;temp1++)
              {
              temp2=temp2+cluster2[temp1];
              }
              centroid2=temp2/i3;
    
               temp2=0;
               for(temp1=0;temp1<i4;temp1++)
              {
               temp2=temp2+cluster3[temp1];
               }
              centroid3=temp2/i4;    

              //Printing clusters 
  
               printf("\n");
               printf("Type 1 customers in cluster 1\t"); 
               printf("\n");

               for(temp1=0;temp1<i2;temp1++)
               {    printf("\t");
               printf("%d",cluster1[temp1]);
                }
               printf("\tCentroid 1=%d",centroid1);


               printf("\n");
               printf("\n");

               printf("Type 2 customers in cluster 2\t");


               printf("\n");


               for(temp1=0;temp1<i3;temp1++)
               {    printf("\t");
                printf("%d",cluster2[temp1]);
               }
               printf("\tcentroid 2=%d",centroid2);
               printf("\n");
               printf("\n");

               printf("Type 3 customers in cluster 3\t");


               printf("\n");


               for(temp1=0;temp1<i4;temp1++)
               {    printf("\t");
               printf("%d",cluster3[temp1]);
               printf("\t");
               }
               printf("\tcentroid 3=%d",centroid3);
               printf("\n");
               printf("\n");

               // Loop terminates after the next calculated mean is equal to the previous mean
               }
               while( (centroid1!=prev_centroid1) || (centroid2!=prev_centroid2) || (centroid3!=prev_centroid3) );
            
			 
               break;
        
		case 0:
			   goto out;
	           //break;
        
		
		default: 
	
	          printf("\n");
              printf("Enter a valid choice for 2 clusters choose 2 , for 3 clusters choose 3");	
              printf("\n");

             
			 
			 
			  }
              //end of switch case of k means 
       
	   
	   }//end of driver while loop for k means 

out: 
break; 

//break;//break out of choose_algo switch case's case 1 

case 2:
                 

                printf("\n");
               printf("**************************************************************Welcome to Hierarchical Clustering Algorithm**************************************************************");
               
			   num_items = process_input(&items, "example.txt");
               scanf("%c",&link);
			   printf("\nEnter type of linkage \nFor AVERAGE_LINKAGE Enter 'a' /* choose average distance */\nFor CENTROID_LINKAGE Enter 't' /* choose distance between cluster centroids */\nFor COMPLETE_LINKAGE Enter 'c' /* choose maximum distance */\nFor SINGLE_LINKAGE  Enter 's' /* choose minimum distance */\n");
		
				
				//printf("\nEnter no of clusters\n");
               // scanf("%d",&k);
				scanf("%c",&link);
				set_linkage(link);
                if (num_items) {
                        cluster_t *cluster = agglomerate(num_items, items);
                        free(items);

                        if (cluster) {
                                fprintf(stdout, "CLUSTER HIERARCHY\n"
                                        "--------------------\n");
                                print_cluster(cluster);
                               printf("\nEnter no of clusters\n");
                               scanf("%d",&k);
                              
                                fprintf(stdout, "\n\n%d CLUSTERS\n"
                                        "--------------------\n", k);
                                get_k_clusters(cluster, k);
                                free_cluster(cluster);
                        }
                }
	
	
	
break;//break out of choose_algo switch case's case 2	

case 3:

printf("\n");
               printf("**************************************************************Welcome to DBSCAN Clustering Alogorithm**************************************************************");
printf("\n");
printf("\n");
	printf("Your Data in text File 'examples' is being Clustered, For any changes, make changes in the the file");
	printf("\n");
	printf("\n");
	
 dbfile =fopen("example.dat", "r");

   // unsigned int 
   num_points = parse_input(dbfile, &points, &epsilon, &minpts);
        	
    if (num_points) {
    	
        dbscan(points, num_points, epsilon,
               minpts, euclidean_dist);
               
        printf("Epsilon: %lf\n", epsilon);
        printf("Minimum points: %u\n", minpts);
        print_points(points, num_points);
        	
    }
    free(points);





break;//break out of choose_algo switch case's case 3

case 0:
	exit(0);
// exit program on user request 
	
	
default:
	
printf("\n");
printf("Enter a valid choice for Kmeans choose 1 for Hierarchical Clustering choose 2 for DBSCAN choose 3");	
printf("\n");	

}//end of choose_algo switch case
}//end of main driver while loop 

return 0;

}
