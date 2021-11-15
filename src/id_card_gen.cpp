#include "id_card_gen.h"
#include <stdio.h>

// libs for debug
#ifdef DEBUG
#ifndef TEST
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#endif

#define ASSETS_FOLDER "assets"
// NOTE: Dear users, please make your templates the same size and change these numbers
#define ID_CARD_WIDTH 2000 
#define ID_CARD_HEIGHT 3173
// Enjoy tuning the text size, turn on DEBUG mode to make it easier for you
#define TEXT_OPACITY 1
#define TEXT_SIZE 300
#define TEXT_Y 2600
unsigned char __TEXT_COLOUR__[] = {0xFF, 0xFF, 0xFF};
#define TEXT_COLOUR __TEXT_COLOUR__
// Enjoy playing with fonts, they are rather crap
#define TEXT_X 20

/**
 * The image is copied before it is modified so that it can be saved to the death
 * NOTE: If you @ me on discord or something I will make the tickets not hard
 * coded.
 * 
 * @param TitoAttendee the attendee to create the id card for.
 * @param std::string the filename for the new id card to be saved as.
 * 
 * @return the return code, 0 is failure and, 1 is success.
 */
static int copyTemplateImage(TitoAttendee attendee,
                              std::string newFileName)
{
    std::string ticketRelease = attendee.getTicket().getTicketRelease();    
    std::string releaseTag = "hacker.png";   
    if (ticketRelease == "Mentor") releaseTag = "mentor.png";
    else if (ticketRelease == "Staff") releaseTag = "staff.png";
    else if (ticketRelease == "Committee") releaseTag = "staff.png";
    //TODO: create a comittee ticket template?
    
    std::string source = ASSETS_FOLDER "/" + releaseTag;

    FILE *src = fopen(source.c_str(), "r"),
         *dest = fopen(newFileName.c_str(), "wb");
    
    // Handle IO errors
    if (src == NULL) {
        std::cerr << "Error IdCard::copyTemplateImage() : Error copying "
                  << source << " to " 
                  << newFileName << ". Source file could not be opened."
                  << std::endl;
        return 0;
    }
    
    if (dest == NULL) {
        std::cerr << "Error IdCard::copyTemplateImage() : Error copying "
                  << source << " to " 
                  << newFileName << ". Destination file could not be opened."
                  << std::endl;
        return 0;
    }
    
    for (int c; c = fgetc(src), c != EOF;) {
        int t = fputc(c, dest);
        
        // Handle IO errors
        if (t == EOF) {            
            std::cerr << "Error IdCard::copyTemplateImage() : Error copying "
                      << source << " to " 
                      << newFileName << ". A write error occurred."
                      << std::endl;
            
            fclose(src);
            fclose(dest);
            return 0;
        }
    }
    
    #ifdef DEBUG
    std::cerr << "Debug IdCard::copyTemplateImage() : Copied "
              << source << " to " 
              << newFileName << std::endl;
    #endif
    
    fclose(src);
    fclose(dest);
    return 1;
}

/**
 * Generates the file name for the image for the id card to be saved as.
 * 
 * Concatenate the ticket id to id_card, this will be fine as there are no
 * duplicate ticket ids. The name of the file is derived from the ticket id to
 * make sure that there are no bad characters in the filename.
 * 
 * @param TitoAttendee the attendee to generate the ticket for.
 * @return the filename for the generated id card to be saved as.
 */
static std::string getFileName(TitoAttendee attendee)
{
    return "id_card_" + std::to_string(attendee.getTicket().getTicketID()) + ".png";
}

IdCard::IdCard(TitoAttendee attendee)
{
    std::string fileName = getFileName(attendee);
    if (!copyTemplateImage(attendee, fileName)) {
        throw ID_CARD_READ_ERROR;
    }
    this->image = CImg<unsigned char> (fileName.c_str());
    
    // Draw text - I hate text centreing so I gave up
    this->image.draw_text(TEXT_X,
                          TEXT_Y,
                          attendee.getName().c_str(),
                          TEXT_COLOUR,
                          0,
                          TEXT_OPACITY,
                          TEXT_SIZE);
    
    this->image.save(fileName.c_str());
    
#ifdef DEBUG
#ifndef TEST
    // Ignore this war crime
    int pid = fork();
    if (pid != 0) {
        CImgDisplay main_disp(this->image, "Preview of the id card");
        
        while (!main_disp.is_closed()) {
            main_disp.wait();
        }
        
        kill(pid, SIGSEGV);
    }
#endif
#endif
}

int IdCard::print()
{
    return 1;
}
