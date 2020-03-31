#ifndef MANAGER_H
#define MANAGER_H

template<class T>
class Manager
{
private:
  std::map<std::string,T> archivos;
  std::list<std::string> dir;
public:
    Manager() {};
  void adddir(std::string dire)
    {
      for(std::list<std::string>::iterator it=dir.begin(); it!=dir.end(); it++)
        {
            if(*it==dire)
            {
              std::cout<<"la direccion "<<dire<<" ya esta agregada"<<std::endl;
                return;
            }
        }
        dir.push_back(dire);
    }
  void removedir(std::string dire)
    {
      for(std::list<std::string>::iterator it=dir.begin(); it!=dir.end(); it++)
        {
            if(*it==dire)
            {
                dir.erase(it);
                return;
            }
        }
      std::cout<<"la direccion "<<dire<<" ya estaba borrada o nunca estuvo"<<std::endl;
    }
  T& get(std::string filename)
    {
      for(typename std::map<std::string,T>::iterator it=archivos.begin(); it!=archivos.end(); it++)
        {
            if(filename==it->first)
            {
                return it->second;
            }
        }
      std::cout<<"agregando "<<filename<<std::endl;
        T t;
        if(t.loadFromFile(filename))
        {
            archivos[filename]=t;
            return archivos[filename]; //no entiendo por que return t no anda pero esto si
        }
        for(std::list<std::string>::iterator it=dir.begin(); it!=dir.end(); it++)
        {
            if(t.loadFromFile(*it+filename))
            {
                archivos[filename]=t;
                return archivos[filename];
            }
        }
        std::cout<<"ERROR el archivo "<<filename<<" no existe"<<std::endl;
        archivos[filename]=t;
        return archivos[filename];
    }
  void remove(std::string filename)
    {
      typename std::map<std::string,T>::iterator it=archivos.find(filename);
        if(it!=archivos.end())
        {
            archivos.erase(it);
            return;
        }
        std::cout<<"ERROR el archivo "<<filename<<" ya fue borrado o nunca estuvo"<<std::endl;
    }
};
#endif // MANAGER_H
