#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

#define INITIAL_CAPACITY 1 << 4

struct hashmap_entry
{
	void *key;
	void *value;
	struct hashmap_entry *next;
};

struct hashmap *hashmap_init(unsigned int key_size, unsigned int value_size, unsigned int (*hashmap_hash)(struct hashmap *, void *))
{
	struct hashmap *map;
	if ((map = malloc(sizeof(struct hashmap))) == NULL)
	{
		return NULL;
	}

	map->capacity = INITIAL_CAPACITY;
	map->size = 0;

	map->key_size = key_size;
	map->value_size = value_size;

	map->hashmap_hash = hashmap_hash;

	if ((map->entries = calloc(map->capacity, sizeof(struct hashmap_entry *))) == NULL)
	{
		return NULL;
	}

	return map;
}

void hashmap_free_entries(struct hashmap_entry **entries, unsigned int capacity)
{
	struct hashmap_entry *tmp, *entry;
	for (int i = 0; i < capacity; i++)
	{
		entry = *(entries + i);
		while (entry != NULL)
		{
			tmp = entry;
			entry = entry->next;
			free(tmp->key);
			free(tmp->value);
			free(tmp);
		}
	}

	free(entries);
}

void hashmap_free(struct hashmap *map)
{
	hashmap_free_entries(map->entries, map->capacity);
	free(map);
}

unsigned int hashmap_hash_int(struct hashmap *map, void *key)
{
	int key_int = *(int *)key;
	if (key_int < 0)
	{
		return -key_int;
	}
	else
	{
		return key_int;
	}
}

void hashmap_rehash(struct hashmap *map)
{
	struct hashmap_entry **old_entries = map->entries;
	unsigned int old_capacity = map->capacity;
	map->capacity = old_capacity << 1;
	map->size = 0;

	if ((map->entries = calloc(map->capacity, sizeof(struct hashmap_entry *))) == NULL)
	{
		return;
	}
	
	for (int i = 0; i < old_capacity; i++)
	{
		for (struct hashmap_entry *entry = *(old_entries + i); entry != NULL; entry = entry->next)
		{
			hashmap_put(map, entry->key, entry->value);
		}
	}

	hashmap_free_entries(old_entries, old_capacity);
}

void *hashmap_get(struct hashmap *map, void *key)
{
	unsigned int index = map->hashmap_hash(map, key) % map->capacity;

	for (struct hashmap_entry *entry = *(map->entries + index); entry != NULL; entry = entry->next)
	{
		if (memcmp(entry->key, key, map->key_size) == 0)
		{
			return entry->value;
		}
	}

	return NULL;
}

void hashmap_put(struct hashmap *map, void *key, void *value)
{
	unsigned int index = map->hashmap_hash(map, key) % map->capacity;

	if (hashmap_get(map, key) != NULL)
	{
		for (struct hashmap_entry *entry = *(map->entries + index); entry != NULL; entry = entry->next)
		{
			if (memcmp(entry->key, key, map->key_size) == 0)
			{
				memcpy(entry->value, value, map->value_size);
				return;
			}
		}
	}

	struct hashmap_entry *entry;

	if ((entry = malloc(sizeof(struct hashmap_entry))) == NULL)
	{
		return;
	}

	if ((entry->key = malloc(map->key_size)) == NULL)
	{
		return;
	}

	if ((entry->value = malloc(map->value_size)) == NULL)
	{
		return;
	}

	memcpy(entry->key, key, map->key_size);
	memcpy(entry->value, value, map->value_size);
	
	entry->next = *(map->entries + index);
	*(map->entries + index) = entry;
	map->size += 1;
	
	if (map->size == map->capacity)
	{
		hashmap_rehash(map);
	}
}
