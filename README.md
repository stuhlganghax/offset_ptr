
# offset_ptr
A simple library for working with raw pointers and foreign structs.
The class provides a simple way to reclass structs from other processes and introduces a uniform interface for accessing the members, no mather whether you are using a internal, extrnal memory reader or something else.

# Example
1. Typedef your pointer type:
```
// Using the default pointer mapper
using ptr_t = reborn::offset_ptr<uint64_t, uint64_t>;
```
2. Define the classes
```
union Position
{
	ptr_t::variable<float, 0x00> x;
	ptr_t::variable<float, 0x04> y;
	ptr_t::variable<float, 0x08> z;
};
union String
{
  ptr_t::variable<uint32_t, 0x00> size;
  ptr_t::variable_cstr<0x04> str;
};
union Entity
{
  ptr_t::variable<String*, 0x14> name;
	ptr_t::variable<Position*, 0x100> position;
};
union Base
{
  ptr_t::variable<Entity*, 0x1122334455667788> entity_player; // players base address
};
```
3. Work with the defined classes:
```
// after getting access to the foreign memory
Base base{getModuleBase()};
// access position
auto pos = base.entity_player->position;
// print position
std::cout << "x=" << *pos->x << " y=" << *pos->y << " z=" << *pos->z << std::endl;
// set x pos
pos->x <<= 50;
// printing the 0 terminated name
std::cout << *base.entity_player->name->str << std::endl;
```
