#ifndef __HASHMAP_HASHMAP_H
#define __HASHMAP_HASHMAP_H

struct hashmap
{
	unsigned int capacity;
	unsigned int size;
	unsigned int key_size;
	unsigned int value_size;
	unsigned int (*hashmap_hash)(struct hashmap *, void *);
	struct hashmap_entry **entries;
};

struct hashmap_entry
{
	void *key;
	void *value;
	struct hashmap_entry *next;
};

struct hashmap_iterator
{
	unsigned int index;
	struct hashmap_entry *entry;
};

struct hashmap *hashmap_init(unsigned int key_size, unsigned int value_size, unsigned int (*hashmap_hash)(struct hashmap *, void *));
void hashmap_free(struct hashmap *map);

void *hashmap_get(struct hashmap *map, void *key);
void hashmap_put(struct hashmap *map, void *key, void *value);

struct hashmap_iterator *hashmap_iterator(struct hashmap *map);
struct hashmap_iterator *hashmap_iterator_next(struct hashmap *map, struct hashmap_iterator *iter);

/* hash functions for common datatypes */
unsigned int hashmap_hash_int(struct hashmap *, void *);

#endif /* __HASHMAP_HASHMAP_H */
