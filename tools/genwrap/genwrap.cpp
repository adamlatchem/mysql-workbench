

#include <glib.h>
#include <string.h>
#include <pcre.h>
#include <algorithm>

#include <grtpp_helper.h>
#include "interfaces/interfaces.h"

using namespace grt;


#define grt_lgpl "/*\n" \
" Generic Runtime Library (GRT)\n" \
" Copyright (c) 2005, 2010, Oracle and/or its affiliates. All rights reserved.\n" \
" This program is free software; you can redistribute it and/or\n" \
" modify it under the terms of the GNU General Public License as\n" \
" published by the Free Software Foundation; version 2 of the\n" \
" License.\n" \
" \n" \
" This program is distributed in the hope that it will be useful,\n" \
" but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n" \
" GNU General Public License for more details.\n" \
" \n" \
" You should have received a copy of the GNU General Public License\n" \
" along with this program; if not, write to the Free Software\n" \
" Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA\n" \
" 02110-1301  USA\n" \
" */\n\n"

/*
void check_requires(char **argv, int argc)
{
  MYX_GRT_ERROR error;
  MYX_GRT_STRUCTS **gstructs= g_new0(MYX_GRT_STRUCTS*, argc);
  GHashTable *file_table;
  int i, j;

  file_table= g_hash_table_new(g_str_hash, g_str_equal);

  for (i= 0; i < argc; i++)
  {
    gstructs[i]= myx_grt_struct_load_list(argv[i], &error);
    if (!gstructs[i])
      fprintf(stderr, "couldn't load file %s\n", argv[i]);

    argv[i]= g_path_get_basename(argv[i]);

    // go through all structs and associate their filenames with the struct
    for (j= 0; j < (int)gstructs[i]->structs_num; j++)
    {
      g_hash_table_insert(file_table,
                          gstructs[i]->structs[j]->name,
                          argv[i]);
    }
  }

  for (i= 0; i < argc; i++)
  {
    GHashTable *files= g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(files, argv[i], argv[i]);
    
    printf("%s\n", argv[i]);
    for (j= 0; j < (int)gstructs[i]->structs_num; j++)
    {
      int k;

      char *name;
      name= gstructs[i]->structs[j]->parent_struct_name;
      if (name)
      {
        char *file= (char*)g_hash_table_lookup(file_table, name);
        if (file)
        {
          if (!g_hash_table_lookup(files, file))
          {
            printf("  <requires file=\"%s\" />\n", file);
            g_hash_table_insert(files, file, file);
          }
        }
        else
          fprintf(stderr, "WARNING: struct '%s' referenced from file '%s' is unknown\n", name, argv[i]);
      }

      for (k= 0; k < (int)gstructs[i]->structs[j]->members_num; k++)
      {
        name= gstructs[i]->structs[j]->members[k].value_type.content_object_struct;
        if (name)
        {
          char *file= (char*)g_hash_table_lookup(file_table, name);
          if (file)
          {
            if (!g_hash_table_lookup(files, file))
            {
              printf("  <requires file=\"%s\" />\n", file);
              g_hash_table_insert(files, file, file);
            }
          }
          else
            fprintf(stderr, "WARNING: struct '%s' referenced from file '%s' is unknown\n", name, argv[i]);
        }

        name= gstructs[i]->structs[j]->members[k].value_type.object_struct;
        if (name)
        {
          char *file= (char*)g_hash_table_lookup(file_table, name);
          if (file)
          {
            if (!g_hash_table_lookup(files, file))
            {
              printf("  <requires file=\"%s\" />\n", file);
              g_hash_table_insert(files, file, file);
            }
          }
          else
            fprintf(stderr, "WARNING: struct '%s' referenced from file '%s' is unknown\n", name, argv[i]);
        }
      }
    }
    printf("\n");
    g_hash_table_destroy(files);
  }
  g_hash_table_destroy(file_table);
}
*/

void* get_mainwindow_impl()
{
  return 0;
}

void register_all_interfaces(grt::GRT *grt)
{
  register_interfaces(grt);
}


void do_generate_interface_classes(grt::GRT *grt, const char *outfile,
                                   const std::vector<std::string> &interfaces)
{
  register_all_interfaces(grt);
  std::vector<grt::Module*> wanted;

  printf("Generating %s\n", outfile);

  for (std::map<std::string,grt::Interface*>::const_iterator intf= grt->get_interfaces().begin();
       intf != grt->get_interfaces().end(); ++intf)
  {
    if (interfaces.empty() || std::find(interfaces.begin(), interfaces.end(), intf->second->name())!=interfaces.end())
    {
      wanted.push_back(intf->second);
    }
  }

  if (!wanted.empty())
  {
    if (!interfaces.empty() && interfaces.size() != wanted.size())
      fprintf(stderr, "WARNING: Some of the specified interfaces were not found\n");

    grt::helper::generate_module_wrappers(grt, outfile, wanted);
  }
  else
    fprintf(stderr, "No interfaces to be wrapped.\n");
}



void generate_interface_classes(const char *header, const char *outfile)
{
  std::vector<std::string> interfaces;
  char line[1024];
  FILE *f= fopen(header, "r");
  if (!f)
  {
    fprintf(stderr, "ERROR: could not open header file '%s'\n", header);
    exit(1);
  }

  const char *errs; int erro;
  pcre *pat= pcre_compile("^\\s*DECLARE_REGISTER_INTERFACE\\(\\s*(\\w+)Impl\\s*,",
                          0, &errs, &erro, NULL);
  if (!pat)
  {
    fprintf(stderr, "ERROR compiling internal regex pattern (%s)\n", errs);
    exit(1);
  }

  while (fgets(line, sizeof(line), f))
  {
    int vec[6];

    if (pcre_exec(pat, NULL, line, static_cast<int>(strlen(line)), 0, 0, vec, 6) == 2)
    {
      char buf[1024];
      pcre_copy_substring(line, vec, 2, 1, buf, sizeof(buf));
      interfaces.push_back(buf);
    }
  }

  grt::GRT grt;
  
  do_generate_interface_classes(&grt, outfile, interfaces);
}



void generate_module_classes(const char *outpath)
{
  puts("NOT IMPLEMENTED");
#if 0
  grt::GRT grt_;
  MYX_GRT *grt= grt_.grt();

  register_all_interfaces(&grt_);
  MYX_GRT_MODULE** list;
  int list_size= 0;
  
  list= g_new0(MYX_GRT_MODULE*, grt->interfaces_num);

  for (unsigned int i= 0; i < grt->interfaces_num; i++)
  {
    MYX_GRT_MODULE *intf= grt->interfaces[i];

    if (interfaces.empty() || is_in_list(intf->name, interfaces))
    {
      list[list_size++]= intf;
    }
  }

  if (list_size > 0)
  {
    MYX_GRT_ERROR err;
    
    if (!interfaces.empty() && interfaces.size() != list_size)
      fprintf(stderr, "WARNING: Some of the specified interfaces were not found\n");

    err= myx_grt_modules_export_wrapper(list, list_size, outfile);
    if (err != MYX_GRT_NO_ERROR)
    {
      fprintf(stderr, "Error generating wrappers: %s\n",
              myx_grt_error_string(err));
      exit(1);
    }
  }
  else
    fprintf(stderr, "No interfaces to be wrapped.\n");

  g_free(list);
#endif
}



void help()
{
  printf("genwrap <command> <options>\n");
  printf("Commands:\n");
  printf("  interfaces <source-header> <output-header-path>\n");
  printf("  wrappers <output-header-path>\n");
}


int main(int argc, char **argv)
{
  if (argc == 1)
  {
    help();
    return 1;
  }

  if (strcmp(argv[1], "interfaces")==0)
  {
    if (argc < 4)
    {
      printf("bad # of arguments\n");
      exit(1);
    }
    generate_interface_classes(argv[2], argv[3]);
  } else
  if (strcmp(argv[1], "wrappers")==0)
  {
    if (argc < 2)
    {
      printf("bad # of arguments\n");
      exit(1);
    }
    //generate_wrapper_classes(argv[2]);
  }
  else
  {
    printf("invalid command %s\n", argv[1]);
    exit(1);
  }
  return 0;
}
