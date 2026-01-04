# JSON Parser
## Documentation
```c
int PVR_json_find_key(char buffer_json[], char key[]);
```
Mostly for inner usage. Searches for a key. Returns index or -1.

```c
char *PVR_json_get_value(char buffer_json[], char key[]);
```
Returns a value by a key or NULL if there's none.
