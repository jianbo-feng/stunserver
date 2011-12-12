/*
   Copyright 2011 John Selbie

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "commonincludes.h"
#include "stuncore.h"
#include "testintegrity.h"


// This test validates that the construction and parsing of the message integrity attribute in a stun message works as expected
// The test also validates both short term and long term credential modes with or without the presence of a fingerprint attribute
HRESULT CTestIntegrity::TestMessageIntegrity(bool fWithFingerprint, bool fLongCredentials)
{
    HRESULT hr = S_OK;
    
    const char* pszUserName = "username";
    const char* pszRealm = "stunrealm";
    const char* pszPassword = "ThePassword";
    
    CStunMessageBuilder builder;
    CStunMessageReader reader;
    uint8_t *pMsg = NULL;
    size_t sizeMsg = 0;
    CStunMessageReader::ReaderParseState state;
    CRefCountedBuffer spBuffer;
    
    builder.AddBindingRequestHeader();
    builder.AddRandomTransactionId(NULL);
    builder.AddUserName(pszUserName);
    builder.AddRealm(pszRealm);
    
    
    if (fLongCredentials == false)
    {
        Chk(builder.AddMessageIntegrityShortTerm(pszPassword));
    }
    else
    {
        Chk(builder.AddMessageIntegrityLongTerm(pszUserName, pszRealm, pszPassword));
    }
    
    if (fWithFingerprint)
    {
        builder.AddFingerprintAttribute();
    }
    
    Chk(builder.GetResult(&spBuffer));
    
    pMsg = spBuffer->GetData();
    sizeMsg = spBuffer->GetSize();
    
    state = reader.AddBytes(pMsg, sizeMsg);
    
    ChkIfA(state != CStunMessageReader::BodyValidated, E_FAIL);
    
    ChkIfA(reader.HasMessageIntegrityAttribute()==false, E_FAIL);
    
    if (fLongCredentials == false)
    {
        ChkA(reader.ValidateMessageIntegrityShort(pszPassword));
    }
    else
    {
        ChkA(reader.ValidateMessageIntegrityLong(pszUserName, pszRealm, pszPassword));
    }
    
Cleanup:
    return hr;
}

HRESULT CTestIntegrity::Run()
{
    HRESULT hr = S_OK;
    
    Chk(TestMessageIntegrity(false, false));
    ChkA(TestMessageIntegrity(true, false));
    
    Chk(TestMessageIntegrity(false, true));
    ChkA(TestMessageIntegrity(true, true));
    
    
Cleanup:
    return hr;
}

