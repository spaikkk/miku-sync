#include "mikusync.hpp"
#define CSIZE_INDEX 0x12
#define FIRST_BLOCK_HEADER 0x33
#define USIZE_INDEX 0x16
#define FILE_START 0x38

using json = nlohmann::json;

uint16_t read_asint16(std::vector<char>& buffer, int index){
    uint16_t value = ((unsigned char)(buffer[index])) | ((unsigned char)(buffer[index+1]))<<8;
    return value;
}

//reads 3 bytes in buffer as a 24 bit integer
uint32_t read_asint24(std::vector<char>& buffer, int index){
    uint32_t value = ((unsigned char)(buffer[index]) | ((unsigned char)(buffer[index+1]))<<8 | 
    ((unsigned char)(buffer[index+2]))<<16);
    return value;
}

uint32_t read_asint32(std::vector<char>& buffer, int index){
    uint32_t value = ((buffer[index]) | ((unsigned char)(buffer[index+1]))<<8 | 
    ((unsigned char)(buffer[index+2]))<<16 | ((unsigned char)(buffer[index+3]))<<24);
    return value;
}

int read_vpr(std::vector<char>& buffer, std::vector<std::string>& messages){
    

    /*calculate the json file size before and after compression by reading
      data from the local fle header*/
    uint32_t uncompr_size = read_asint32(buffer, USIZE_INDEX);
    fmt::print("Dimensione prodotta da index: {}\n", uncompr_size);
    uint32_t compr_size = read_asint32(buffer, CSIZE_INDEX);
    uint32_t ending = FIRST_BLOCK_HEADER + compr_size - 1;
    fmt::print("Uncompressed size: {}, comrpessed size: {}", uncompr_size, compr_size);

    int rem_counter = 0;
    int check = 1;
    int next_block = FIRST_BLOCK_HEADER;
    int block_size = 0;
    
    
    while(check){
        block_size = read_asint16(buffer, next_block+1);
        std::string message = fmt::format("Size: {}, Pos: {}", block_size, next_block);
        std::string confirm;
        //fmt::print("Size = {:x}, pos: {:x}\n", block_size, next_block);
        if(buffer[next_block]==0x00){
            //fmt::print("Block succesfully deleted\n");
            confirm+="Block successfully deleted";
        }else if(buffer[next_block]==0x01){
            check = 0;
        }else{
            //std::cerr << "ERROR: failed reading block headers" << std::endl;
            confirm+="ERROR: failed reading block headers";
            return 1;
        }
        messages.push_back(message);
        messages.push_back(confirm);
        for(int n=0; n<5; n++){
            buffer.erase(buffer.begin()+next_block);
        }
        next_block+=block_size;
        rem_counter+=5;
    }

    buffer.erase(buffer.begin()+uncompr_size+FIRST_BLOCK_HEADER, buffer.end());
    buffer.erase(buffer.begin(), buffer.begin()+FIRST_BLOCK_HEADER);
    return 0;
}


/*
    Json Parsing
*/

int processa_json(std::vector<char>& buffer, std::vector<track>& tracks){
    int cont_prova = 0;
    int stop_prova = 0;
    std::ofstream testxt("test.txt");
    json parsed_data = json::parse(buffer.begin(), buffer.end());
    for(auto& obj : *parsed_data.find("tracks")){
        track new_track;
        new_track.name = obj["name"];
        
        for (auto& part_it : *obj.find("parts")){
            part new_part;
            new_part.name = part_it["name"];
            for(auto& note_it : *part_it.find("notes")){
                note new_note;
                new_note.lyric = note_it["lyric"];
                new_note.duration = note_it["duration"];
                new_part.notes.push_back(new_note);
            }
            new_track.parts.push_back(new_part);
        }
        tracks.push_back(new_track);
    }
    
    int track_count = 0;
    int part_count = 0;
    for(auto it_track : tracks){
        testxt << "Traccia n " << track_count << ", Nome: " << it_track.name << std::endl;
        track_count++;
        for(auto it_part : it_track.parts){
            testxt << "     Parte n " << part_count << ", Nome: " << it_part.name << std::endl;
            part_count++;
            for(auto it_note : it_part.notes){
                testxt << "         Lyric: " << it_note.lyric << ", duration: " << it_note.duration << std::endl;
            }
        }
    }
    return 1;
}