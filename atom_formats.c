#include <config.h>

#include <io.h>
#include <atl.h>
#include <atom_formats.h>

IOField Atom_send_get_msg_flds[] =
{
{"atom_string", "string", sizeof(char *), IOOffset(send_get_atom_msg_ptr,
						   atom_string)},
    {"atom", "integer", sizeof(atom_t), IOOffset(send_get_atom_msg_ptr,
						 atom)},
    {(char *) 0, (char *) 0, 0, 0}
};

IOField Atom_init_contact_msg_flds[] =
{
    {"send_id", "integer", sizeof(int), IOOffset(init_contact_msg_ptr,
						   send_id)},
    {"send_list", "integer", sizeof(int), IOOffset(init_contact_msg_ptr,
						   send_list)},
    {(char *) 0, (char *) 0, 0, 0}
};

IOField atom_provisional_use_msg_flds[] =
{
    {"domain", "string", sizeof(char *),
     IOOffset(provisional_use_msg_ptr, domain)},
    {"time", "integer", sizeof(int), 
     IOOffset(provisional_use_msg_ptr, time)},
    {(char *) 0, (char *) 0, 0, 0,}
};
