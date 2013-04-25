import os
import sys
import shutil
import codecs
import win32com.client


class ItunesPlaylist:
    def __init__(self, playlist_file_path, playlist_base_path=None):

        self._playlist_base_path = playlist_base_path
        self._playlist_file_path = playlist_file_path
        self._playlist_items = []
        self._parse_playlist_file()
        
        
    def _parse_playlist_file(self):
        try:
            
            playlist_file_lines = codecs.open(self._playlist_file_path, encoding='utf_8').readlines()[1:]
            
        except:
            raise Exception('Unable to open playlist file')
            
           
        plalist_file_paths = [playlist_file_line.split(u'\t')[-1].strip() for \
                              playlist_file_line in playlist_file_lines]
        
#        We now have a list of absolute paths of the files in the play list. If we are to 
#        use them for comparison of file paths in another base directory, we are to strip
#        the common path from all of the files in the playlist:

        if self._playlist_base_path is None:
            self._playlist_base_path = os.path.commonprefix(plalist_file_paths)
        self._playlist_items = [file_path[len(self._playlist_base_path) + len(os.path.sep):] \
                                              for file_path in plalist_file_paths]
        
#        Remove duplicates
        self._playlist_items = set(self._playlist_items)

    def get_playlist_items(self):
        return self._playlist_items
    
class ItunesPlaylistCOM:
    
#    Some info:
#    1. To get the xml path of iTunes library, use the following function: itunes.LibraryXMLPath
#    2. To get the folder path of iTunes' source:
#    urllib.url2pathname(urllib.splittype(element.childNodes[20].firstChild.data)[1]), where element
#    is the first <dict> element

    
    def __init__(self, playlist_name):

        self._playlist_items = set()
        

#        Declaring a local itunes object using the COM interface            
        itunes = win32com.client.Dispatch('itunes.Application')            
        self._playlistObj = itunes.LibrarySource.Playlists.ItemByName(playlist_name)
        songs_count = self._playlistObj.Count
        
        while songs_count != 0:
            self._playlist_items.append(self._playlistObj.Item(songs_count).Location)
            
            
    def get_playlist_items(self):
        return self._playlist_items


class ManagedDirectory:
    def __init__(self, base_directory):
        if not os.path.isdir(base_directory):
            raise Exception('%s is not a valid directory!' % (base_directory))
        
        self._base_directory = base_directory
        
    def get_playlist_item_full_path(self, playlist_item):
        return os.path.join(os.path.dirname(self._base_directory), playlist_item)
    
    def get_items_in_base_directory(self):
        items = []
        directory_iterator = os.walk(self._base_directory)
        try:
            while True:
                dir_items = directory_iterator.next()
                items_to_append = [os.path.join(dir_items[0], file_name) for file_name in dir_items[2]]
                items.append(*items_to_append)
        except :
            pass
        
        items = [file_path[len(self._base_directory) + len(os.path.sep):] for file_path in items]
#        Removing duplicates
        return set(items)


def split_path(path):

    folders = []
    while 1:
        path, folder = os.path.split(path)

        if folder != "":
            folders.append(folder)
        else:
            if path != "":
                folders.append(path)
            break

    folders.reverse()

    return folders
    
def print_usage():
    print ('Usage: %s <source directory> <destination directory> [playlist 1] [playlist 2] ...' % (os.path.basename(sys.argv[0])))




def main():
    
#    Checking parameters
    
    if len(sys.argv) < 4:        
        print_usage()
        return 1
    
    source_dir_path = sys.argv[1]
    dest_dir_path = sys.argv[2]
    
    if not os.path.isdir(source_dir_path) or not os.path.isdir(dest_dir_path):
        print_usage()
        return 1
    

    unified_playlists_items = set()
    for playlist_file_path in sys.argv[3:]:
        unified_playlists_items.update(ItunesPlaylist(playlist_file_path, source_dir_path).get_playlist_items())
        

    dest_dir = ManagedDirectory(dest_dir_path)
    
    dest_dir_existing_items = dest_dir.get_items_in_base_directory()
    
    to_remove_items = dest_dir_existing_items - unified_playlists_items
    for item in to_remove_items:
        item_path = dest_dir.get_playlist_item_full_path(item)
        os.remove(item_path)

#        If the directory is now empty, remove it
        if os.listdir(os.path.dirname(item_path)) == []:
            os.rmdir(os.path.dirname(item_path))

#    Copying the unified list of playlist items to the destination directory 
    
    for playlist_item in unified_playlists_items:
        dest_item_path = os.path.join(dest_dir_path, playlist_item)
        dest_item_dir = os.path.dirname(dest_item_path)
        if not os.path.exists(dest_item_dir):
            os.makedirs(dest_item_dir)
        
        shutil.copy(os.path.join(source_dir_path, playlist_item), dest_item_path)
    
    return 0

if __name__ == '__main__':
    sys.exit(main())
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
