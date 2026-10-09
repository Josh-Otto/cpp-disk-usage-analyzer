// FolderInfo.cpp

#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "sort.h"
#include "Folder.h"

using namespace std;

string getFolderName(const filesystem::path& path)
{
    if(path.filename().empty())
        return path.parent_path().filename().string();
    return path.filename().string();
}

static bool isValidPath(const filesystem::path path)
{
    if(filesystem::exists(path))
        return true;
    else
        return false;
}

static bool isValidFolder(filesystem::path& curDir,string& arg)
{
    if(filesystem::exists((filesystem::path)(curDir.string() + "\\" + arg)))
        return true;
    else
        return false;
}

static vector<Folder> sizeOfDirectory(string folderPath,const filesystem::path& dirPath,bool b,size_t CLUSTER_SIZE, bool& isPath) // add pointer to folders vector for multiple folders
{
    vector<Folder> folders;
    size_t total{0};

    try
    {
        for(auto& entry : filesystem::directory_iterator( dirPath, filesystem::directory_options::skip_permission_denied))
        {
            // VARIABLES
            Folder f;
            error_code ec1;

            //------------------------------------------------- REGULAR FILE READ -----------------------------------------------
            if(filesystem::is_regular_file(entry.path(),ec1))
            {
                // GET THE FILE SIZE
                uint64_t folderSize = filesystem::file_size(entry.path(),ec1);

                // ERROR = RESET VALUE OF FILE
                if(ec1)
                    folderSize = 0;

                // Add to folderSize AND total
                if (!b)
                {
                    if(folderSize % CLUSTER_SIZE != 0)
                    {
                        total += folderSize / CLUSTER_SIZE +1;
                    } 
                    else
                    {
                        total += folderSize / CLUSTER_SIZE;
                    }
                } 
                else
                    total += folderSize;
            } 
            //---------------------------------------------- END OF REGULAR FILE READ -------------------------------------------
            else
            //----------------------------------------------------- FOLDER READ -------------------------------------------------
            {
                error_code ec2;

                if (isPath)
                {
                    f.path = dirPath;
                    f.path += getFolderName(entry.path());
                }
                else
                {
                    f.path = folderPath;
                    f.path += "\\";
                    f.path += getFolderName(entry.path());
                }

                filesystem::recursive_directory_iterator it(entry.path(), filesystem::directory_options::skip_permission_denied, ec2);

                filesystem::recursive_directory_iterator end;

                while(it != end)
                {
                    if(ec2)
                    {
                        ec2.clear();
                        it.increment(ec2);
                        continue;
                    }

                    if(filesystem::is_regular_file(it->path(),ec2))
                    {
                        uint64_t folderSize = filesystem::file_size(it->path(),ec2);

                        if(ec2)
                        {
                            folderSize = 0;
                            ec2.clear();
                        }

                        if(!b)
                        {
                            if(folderSize % CLUSTER_SIZE != 0)
                                f.size += folderSize / CLUSTER_SIZE + 1;
                            else
                                f.size += folderSize / CLUSTER_SIZE;
                        } 
                        else
                        {
                            f.size += folderSize;
                        }
                    }

                    it.increment(ec2);
                }
                total += f.size;
                folders.push_back(f);
            }
            //------------------------------------------------- END OF FOLDER READ ----------------------------------------------
        }

        Folder total_of_folders;
        if(isPath)
        {
            total_of_folders.path = dirPath;
        }
        else
        {
            total_of_folders.path = dirPath.filename();
        }
        total_of_folders.size = total;
        folders.push_back(total_of_folders);

        return folders;
    } 
    catch(const std::exception& ex)
    {
        cout << endl << ex.what() << dirPath << endl;
        return folders;
    }
}

int main(int argc,char* argv[])
{
    // 5 specifiers in input argv[1] -> arv[5]
    vector<string> inputArray;

    filesystem::path currentDirectory = filesystem::current_path();

    vector<Folder> folders;

    // all first 
    bool s = false;
    bool k = false;
    bool h = false;
    bool z = false;
    bool n = false;
    bool r = false;
    bool b = false;

    bool BOOL_TRUE = true;
    bool BOOL_FALSE = false;

    bool bh_trigger = false;

    const uint64_t KiB = 1024ULL;
    const uint64_t MiB = KiB * 1024;
    const uint64_t GiB = MiB * 1024;
    const uint64_t TiB = GiB * 1024;

    bool help = false;
    bool version = false;
    string versionID = "1.1.0";

    size_t CLUSTER_SIZE = 4096; // 4096 bytes -> allow this to change to 512 or 2048


    // INPUT PROCESSING
    try
    {
        for(size_t i = 1; i < argc; ++i) // ITERATES THROUGH ALL ARGV[]
        {
            string arg = argv[i];
            inputArray.push_back(arg);

            if(arg[0] == '-' && arg.length() > 1)
            {
                //The char index of the <string> arg
                if(arg[1] != '-')
                {
                    for(size_t index = 1; index < arg.length(); ++index)
                    {
                        if(arg[index] == 's')//         s
                            s = true;
                        else if(arg[index] == 'k')//    k
                        {
                            k = true;
                            if(CLUSTER_SIZE != 4096)
                                throw runtime_error("Switches -k and --block-size=dddd cannot be used at the same time");
                            CLUSTER_SIZE = 1024;
                        } 
                        else if(arg[index] == 'h')//    h
                        {
                            h = true;
                            b = true;
                            bh_trigger = !bh_trigger;
                        }
                        else if(arg[index] == 'z')//    z
                            z = true;
                        else if(arg[index] == 'n')//    n
                            n = true;
                        else if(arg[index] == 'r')//    r
                            r = true;
                        else if(arg[index] == 'b')//    b
                        {
                            b = true;
                            bh_trigger = !bh_trigger;
                        }
                        else
                        {
                            throw runtime_error(string("Invalid switch <") + arg[index] + '>');
                        }
                    }
                }
                else
                {
                    if(arg == "--help")
                    {
                        help = true;
                    } 
                    else if(arg == "--version")
                    {
                        version = true;
                    } 
                    else if(arg.size() > 13)// set cluster-size to a 4 or less digit number (unsigned)
                    {
                        if(arg[2] == 'b' && arg[3] == 'l' && arg[4] == 'o' && arg[5] == 'c' && arg[6] == 'k' && arg[7] == '-' && arg[8] == 's' && arg[9] == 'i' && arg[10] == 'z' && arg[11] == 'e' && arg[12] == '=')
                        {
                            if(k)
                                throw runtime_error("Switches -k and --block-size=dddd cannot be used at the same time");
                            try 
                            {
                                string number;
                                int value = 0;

                                for(int i = 13; i <= arg.size(); ++i)
                                {
                                    number = number + arg[i];
                                }
                                value = stoi(number);
                                if(value > 0)
                                {
                                    CLUSTER_SIZE = value;
                                } else
                                    throw runtime_error("Error when converting --block-size=" + s);
                            }
                            catch (...)
                            {
                                throw runtime_error("Improper use of --block-size switch");
                            }
                        }
                    } 
                    else
                    {
                        throw runtime_error("Unknown command <" + arg + ">");
                    }
                }
            }
            else
            {
                std::string s = arg;

                try
                {
                    if(isValidPath(arg))
                    {
                        if(s.back() == '\\')
                        {
                            folders = sizeOfDirectory(arg, arg,b,CLUSTER_SIZE,BOOL_TRUE);
                            continue;
                        } 
                        else
                        {
                            folders = sizeOfDirectory(arg,(arg + '\\'),b,CLUSTER_SIZE,BOOL_TRUE);
                            continue;
                        }
                    }
                } 
                catch(...)
                {
                    //nothing
                }
                
                try
                {
                    if(isValidFolder(currentDirectory,arg))
                    {
                        folders = sizeOfDirectory(arg, (currentDirectory.string() + '\\' + arg),b,CLUSTER_SIZE,BOOL_FALSE);
                        continue;
                    }
                } 
                catch(...)
                {
                    throw runtime_error("Invalid folder <" + arg + ">");
                }

                //throw
            }
        }

        if(n && z)
        {
            throw runtime_error("Switches -n and -z are incompatible");
        }
        if(b && !bh_trigger)
        {
            throw runtime_error("Switches -b and -h cannot be used at the same time");
        }
        if(version && help)
        {
            throw runtime_error("Switches --version and --help are incompatible");
        }
        if(version)
        {
            cout << versionID << endl;
            return 0;
        }
        if(help)
        {
            cout << endl << "du (c) 2026, Joshua Otto" << endl;
            cout << "====================================================" << endl;
            cout << "Version " << versionID << endl << endl;;
            cout << "A disk usage utility inspired by the UNIX du command." << endl << endl;
            cout << "Examples:" << endl;

            cout << "  du" << endl;
            cout << "    > display the sum of the cluster sizes of each directory" << endl;
            cout << "    starting the cwd" << endl << endl;

            cout << "  du folder" << endl;
            cout << "    > display the sum of the cluster sizes of each directoy" << endl;
            cout << "      starting with 'folder'" << endl << endl;
            
            cout << "  du -h" << endl;
            cout << "    > display the results in a human readable form" << endl << endl;

            cout << "  du -s" << endl;
            cout << "    > display only the final summary" << endl << endl;

            cout << "  du -b" << endl;
            cout << "    > display in bytes" << endl << endl;

            cout << "  du -k" << endl;
            cout << "    > cluster size is 1024" << endl << endl;

            cout << "  du -z" << endl;
            cout << "    > display the list sorted by size" << endl << endl;

            cout << "  du -n" << endl;
            cout << "    > display the list sorted by name" << endl << endl;

            cout << "  du -r" << endl;
            cout << "    > display the list in reverse order" << endl << endl;

            cout << "  du --block-size=dddd" << endl;
            cout << "    > set the cluster size to the specified integer > 0" << endl << endl;

            cout << "  du --help" << endl;
            cout << "    > displays the help" << endl << endl;

            cout << "  du --version" << endl;
            cout << "    > displays version number in the format d.d.d" << endl << endl;

            return 0;
        }
    } 
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 0;
    }

    // OUTPUT

    if(s && folders.empty())
    {
        folders = sizeOfDirectory("",currentDirectory,b,CLUSTER_SIZE,BOOL_FALSE);
        cout << folders.back().size << endl;
        return 0;
    }
    else if(s)
    {
        cout << folders.back().size << endl;
        return 0;
    }
    
    
    if(folders.empty())
    {
        folders = sizeOfDirectory("",currentDirectory,b,CLUSTER_SIZE,BOOL_FALSE);
        folders.back().path = "";

        if(z)
        {
            pleaseSortThisBySizeASC(folders);
        }
        if(r)
        {
            reverse(folders.begin(),folders.end());
        }


        if (!h)
        {
            // Display for clusters or bytes
            for(size_t i = 0; i < folders.size(); ++i)
            {
                auto& f = folders[i];
                printf("%-11llu",f.size);

                cout << "." << f.path.string() << endl;
            }
        }
        else
        {
            if(z)
            {
                pleaseSortThisBySizeASC(folders);
            }
            if(r)
            {
                reverse(folders.begin(),folders.end());
            }
            string unit;
            // Unit Display for Human Readable Switch
            for(size_t i = 0; i < folders.size(); ++i)
            {
                auto& f = folders[i];

                if(f.size >= 100*GiB)
                {
                    f.size /= TiB;
                    unit = "T";
                } else if(f.size >= 100*MiB)
                {
                    f.size /= GiB;
                    unit = "G";
                } else if(f.size >= 100*KiB)
                {
                    f.size /= MiB;
                    unit = "M";
                } else if(f.size >= 100)
                {
                    f.size /= KiB;
                    unit = "K";
                } else
                {
                    unit = "B";
                }

                string str = std::to_string(f.size);
                str = str + unit;
                const char* cstr = str.c_str();

                printf("%-10s",cstr);

                cout << "." << f.path.string() << endl;
            }
        }

        //cout << currentDirectory.string() << "\t\t\t" << sizeOfDirectory(currentDirectory,b,CLUSTER_SIZE);
    }
    else
    {
        if(!h)
        {
            // Display for clusters or bytes
            for(auto& f : folders)
            {
                printf("%-11llu",f.size);
                cout << f.path.string() << endl;
            }
        } 
        else
        {
            std::string unit;
            // Unit Display for Human Readable Switch
            for(auto& f : folders)
            {
                if(f.size >= 100*GiB)
                {
                    f.size /= TiB;
                    unit = "T";
                } else if(f.size >= 100*MiB)
                {
                    f.size /= GiB;
                    unit = "G";
                } else if(f.size >= 100*KiB)
                {
                    f.size /= MiB;
                    unit = "M";
                } else if(f.size >= 100)
                {
                    f.size /= KiB;
                    unit = "K";
                } else
                {
                    unit = "B";
                }

                string str = std::to_string(f.size);
                str = str + unit;
                const char* cstr = str.c_str();

                printf("%-10s",cstr);

                cout << f.path.string() << endl;
            }
        }
    }
    return 0;
}
/*
      The program should have the following command-line interface:
                                                        ----------     du [-v] [--help] [--version] [--block-size=dddd] [folder]*
                                                        ----------     du [-skhznrb] [--help] [--version] [--block-size=dddd] [folder]*
                                                        ----------     du [-skhznrb] [--help] [--version] [--block-size=dddd] [folder]*

        FIND OUT WHICH ONES YOU HAVE COMPLETED.

      -k Set the cluster size to 1024
      -s Display only the final summary of each root folder
      -b Output in bytes,not clusters
      --help Display the help listing
      --version Display the version number only in the form d.d.d
      [folder]* If no folder is provided the current working directory is used.Otherwise,the
            specified folder is where the scan begins.
        
        // DONE ALL ABOVE
        // NOT FINISHED ALL BELOW

      -h Make the output human readable.Output is in KiB,MiB,GiB,TiB3 rounded to either
            one decimal place if less than 10 or zero decimal places otherwise.A zero value has
            neither a decimal place nor a unit. (see demo).

      -z Sort rows by size
      -n Sort rows by folder name
      -r Display rows in reverse order

      --block-size = dddd Set the cluster size to the specified value > 0.


*/