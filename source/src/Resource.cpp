
#include <dirent.h>
#include <SDL_image.h>

#include "Resource.h"

#include "Global.h"
#include "Game.h"

////Carregar uma imagem
SDL_Texture *loadImageToMemory(const std::string &fileName)
{
    printf("LOADING: %s\n", fileName.c_str()); //Debug

    SDL_Texture* img = IMG_LoadTexture(Game::getGlobalGame()->getRenderer(), fileName.c_str());

	/*If loading failed*/
	if(img == nullptr)
	{
		fprintf(stderr, "\nError loading image: %s\n", IMG_GetError());
		return nullptr;
	}

	return img;
}


Resource::Resource()
{

}

Resource::~Resource()
{
	std::map<std::string, Image*>::iterator iter;
	std::map<std::string, Font*>::iterator iter2;

	iter = images.begin();
	while (iter != images.end())
	{
		std::cout << "OUTPUT: Deleting resource with key: " <<iter->first <<std::endl;

		delete iter->second;
		images.erase(iter);
		//advance(iter, 1);		//Why the hell do I have to advance one more unit?
		iter = images.begin();
	}

	iter2 = fonts.begin();
	while(iter2 != fonts.end())
	{
		std::cout << "OUTPUT: Deleting resource with key: " <<iter2->first <<std::endl;

		delete iter2->second;
		fonts.erase(iter2);
		//advance(iter2, 1);
		iter2 = fonts.begin();
	}
}

Image* Resource::AddImage(const char *path, const std::string& key)
{
	std::map<std::string, Image*>::iterator iter;
	std::fstream file;

    iter = images.find(key);

	//If the image have not been allocated yet
	if(iter == images.end())
	{
		//Load the file and select the images to be loaded
		file.open(path, std::fstream::in);
		std::string tag, content;

		//Search for the correct key
        //BUG: When the file has a blank line before EOF
		while(!file.eof())
		{
			file >> tag;

			if(tag.compare("key:") == 0)
			{
				file >> content;

				//Did found the key: Load the image and return
				if(content.compare(key) == 0)
				{
					return LoadImage(file, key);
				}
			}
		}

		file.close();
	}
	else
    {
		std::cout << "OUTPUT: Image with key '" <<key <<"' from file '" <<path <<" was already found in resources" <<std::endl;
		return iter->second;
    }

    std::cerr << "ERROR: Could not load image with key '" <<key <<"' from file '" <<path <<std::endl;

    return nullptr;
}

Image* Resource::LoadImage(std::fstream &file, const std::string& key)
{
	std::string tag = " ";
	std::string path = "\0";
	int nLines, nCols, clipWidth, clipHeight, margin, padding, centerX=0, centerY=0;
    int baseFrames = 1;
	int success = 0;


	while(tag.compare("!") != 0)
	{
		file >> tag;

		if(tag.compare("file:") == 0)
		{
			file >> path;
			success++;
		}
		else if(tag.compare("clip:") == 0)
		{
			file >> nLines;
			file >> nCols;
			file >> clipWidth;
			file >> clipHeight;
            file >> margin;
            file >> padding;
			success++;
		}
		else if (tag.compare("center:") == 0){
            file >> centerX;
            file >> centerY;
		}
        else if(tag.compare("clip:") == 0)
        {
            file >> baseFrames;
        }
	}

	if(success != 2)
	{
		std::cerr << "ERROR: Could not load image completely" <<std::endl;
		return nullptr;
	}

    SDL_Rect *rect = Image::createClip(nLines, nCols, clipWidth, clipHeight, margin, padding);
    Image *newImage = new Image(loadImageToMemory(path), nLines*nCols, rect, centerX, centerY);
    newImage->setBaseFrame(baseFrames);
    //newImage->setFrameWidth(clipWidth-(margin+padding));

	images.insert(std::pair<std::string, Image*>(key, newImage));

	return newImage;
}

void Resource::AddFont(const char *path, const std::string& key)
{
	std::map<std::string, Font*>::iterator iter;
	std::fstream file;
	int success = 0;

	iter = fonts.find(key);

	//If the image have not been allocated yet
	if(iter == fonts.end())
	{
		//Load the file and select the fonts to be loaded
		file.open(path, std::fstream::in);
		std::string tag, content;

		//Search for the correct key
		while(!file.eof())
		{
			file >> tag;

			if(tag.compare("key:") == 0)
			{
				file >> content;

				//Did found the key: Load the image and return
				if(content.compare(key) == 0)
				{
					LoadFont(file, key);
					success = 1;
				}
			}
		}

		file.close();
	}

	if(success == 0)
		std::cout << "OUTPUT: Font with key '" <<key <<"' from file '" <<path <<" was already found in resources" <<std::endl;
	if(success == -1)
		std::cerr << "ERROR: Could not load font with key '" <<key <<"' from file '" <<path <<std::endl;
}

void Resource::LoadFont(std::fstream &file, const std::string& key)
{
	std::string tag = "\0";
	std::string path = "\0";
	int ptSize = 0;
	int success = 0;

	while(tag.compare("!") != 0)
	{
		file >> tag;

		if(tag.compare("file:") == 0)
		{
			file >> path;
			success++;
		}
		else if(tag.compare("size:") == 0)
		{
			file >> ptSize;
			success++;
		}
	}

	if(success != 2)
	{
		std::cerr << "ERROR: Could not load font completely" <<std::endl;
		return;
	}

	fonts.insert(std::pair<std::string, Font*>(key, new Font(TTF_OpenFont(path.c_str(), ptSize), ptSize)));
}

Image* Resource::GetImage(const std::string& key)
{
	std::map<std::string, Image*>::iterator iter;

	iter = images.find(key);
	if(iter != images.end())
	{
		//std::cout <<"OUTPUT: Loaded image with key '" <<iter->first <<"'"  << std::endl;

		return iter->second;
	}

	return nullptr;
}

Font* Resource::GetFont(const std::string& key)
{
	std::map<std::string, Font*>::iterator iter;

	iter = fonts.find(key);
	if(iter != fonts.end())
	{
		std::cout <<"OUTPUT: Loaded font with key '" <<iter->first <<"': " <<std::endl;

		return iter->second;
	}

	return nullptr;
}

void Resource::getListOfFiles(std::vector<std::string> &myVec, std::string insideDirectory, std::string extension)
{
    DIR *dir;
    dirent *ent;

    if ((dir = opendir(insideDirectory.c_str())) != nullptr)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != nullptr)
        {
            int len = strlen(ent->d_name);
            for (int i = 0, L = extension.size(); i < L; ++i)
            {
                if ( ent->d_name[len-L+i] != extension.at(i)){
                    len = -1;
                    break;
                }
            }

            // Proximo arquivo
            if (len == -1)  continue;

            myVec.push_back( insideDirectory+std::string(ent->d_name) );
        }

        closedir (dir);
    }
    else
    {
        printf("Could not open directory %s", insideDirectory.c_str());
    }
}
