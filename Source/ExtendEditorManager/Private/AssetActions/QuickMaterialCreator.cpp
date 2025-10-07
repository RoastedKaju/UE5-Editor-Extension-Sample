// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreator.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"

void UQuickMaterialCreator::CreateMaterialFromSelectedTextures()
{
	if (bUseCustomName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("Please enter a valid name"), false);
			return;
		}
	}

	const TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTextures;
	FString SelectedTexturePath;
	uint32 PinCounter = 0;

	const bool bSuccess = ProcessSelectedData(SelectedAssets, SelectedTextures, SelectedTexturePath);
	if (!bSuccess) return;

	if (CheckIsNameUsed(SelectedTexturePath, MaterialName))
	{
		return;
	}

	UMaterial* CreatedMaterial = CreateMaterialAsset(MaterialName, SelectedTexturePath);

	if (!IsValid(CreatedMaterial))
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("Failed to create material"), true);
		return;
	}

	for (const auto& Texture : SelectedTextures)
	{
		if (IsValid(Texture))
		{
			switch (ChannelPackingType)
			{
			case EChannelPackingType::NoChannelPacking:
				Default_CreateNewMaterialNode(CreatedMaterial, Texture, PinCounter);
				break;
			case EChannelPackingType::ORM:
				ORM_CreateNewMaterialNode(CreatedMaterial, Texture, PinCounter);
				break;
			case EChannelPackingType::MAX:
				break;
			}
		}
	}

	if (PinCounter > 0)
	{
		DebugHelper::ShowNotification(TEXT("Successfully connected ") + FString::FromInt(PinCounter) + TEXT(" pins."));
	}

	if (bCreateMaterialInstance)
	{
		CreateMaterialInstanceConstant(CreatedMaterial, MaterialName, SelectedTexturePath);
	}
}

bool UQuickMaterialCreator::ProcessSelectedData(const TArray<FAssetData>& SelectedData, TArray<UTexture2D*>& OutTextures, FString& PackagePath)
{
	if (SelectedData.Num() == 0)
	{
		DebugHelper::ShowDialogMessage(EAppMsgType::Ok, TEXT("No Assets Selected"), true);
		return false;
	}

	bool bMaterialNameSet = false;
	for (const FAssetData& AssetData : SelectedData)
	{
		UObject* SelectedAssetObject = AssetData.GetAsset();

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAssetObject);

		if (!SelectedTexture)
		{
			DebugHelper::ShowDialogMessage(EAppMsgType::Ok, AssetData.AssetName.ToString() + " Is Not a Texture, select only textures", true);
			return false;
		}

		OutTextures.Add(SelectedTexture);

		if (PackagePath.IsEmpty())
		{
			PackagePath = AssetData.PackagePath.ToString();
		}

		if (!bUseCustomName && !bMaterialNameSet)
		{
			MaterialName = SelectedAssetObject->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName.InsertAt(0, TEXT("M_"));
			bMaterialNameSet = true;
		}
	}

	return true;
}

bool UQuickMaterialCreator::CheckIsNameUsed(const FString& FolderToCheck, const FString& NameToCheck) const
{
	const TArray<FString> ListedAssets = UEditorAssetLibrary::ListAssets(FolderToCheck, false);

	for (const auto& Asset : ListedAssets)
	{
		const FString AssetName = FPaths::GetBaseFilename(Asset);

		if (AssetName.Equals(MaterialName))
		{
			DebugHelper::ShowDialogMessage(EAppMsgType::Ok, MaterialName + TEXT(" Name is already in use"), true);
			return true;
		}
	}

	return false;
}

UMaterial* UQuickMaterialCreator::CreateMaterialAsset(const FString& InMaterialName, const FString& Path)
{
	// Load asset tools module
	const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

	// Create Asset and Factory
	UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
	UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(InMaterialName, Path, UMaterial::StaticClass(), Factory);

	return Cast<UMaterial>(CreatedAsset);
}

void UQuickMaterialCreator::Default_CreateNewMaterialNode(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);

	if (!TextureSampleNode) return;

	if (!CreatedMaterial->HasBaseColorConnected())
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (!CreatedMaterial->HasMetallicConnected())
	{
		if (TryConnectMetallic(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (!CreatedMaterial->HasRoughnessConnected())
	{
		if (TryConnectRoughness(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (!CreatedMaterial->HasNormalConnected())
	{
		if (TryConnectNormal(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (!CreatedMaterial->HasAmbientOcclusionConnected())
	{
		if (TryConnectAO(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}
}

void UQuickMaterialCreator::ORM_CreateNewMaterialNode(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);

	if (!TextureSampleNode) return;

	// Base color
	if (!CreatedMaterial->HasBaseColorConnected())
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}
	// Normal
	if (!CreatedMaterial->HasNormalConnected())
	{
		if (TryConnectNormal(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}
	// ORM
	if (!CreatedMaterial->HasRoughnessConnected())
	{
		if (TryConnectORM(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter += 3;
			return;
		}
	}
}

bool UQuickMaterialCreator::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const auto& BaseColorName : BaseColorArray)
	{
		if (SelectedTexture->GetName().Contains(BaseColorName))
		{
			TextureSampleNode->Texture = SelectedTexture;
			UMaterialEditorOnlyData* MaterialEditorData = CreatedMaterial->GetEditorOnlyData();
			if (MaterialEditorData)
			{
				// Add the texture sample node to the material graph
				MaterialEditorData->ExpressionCollection.Expressions.Add(TextureSampleNode);

				// Connect the node to BaseColor
				FExpressionInput& BaseColorInput = CreatedMaterial->GetEditorOnlyData()->BaseColor;
				BaseColorInput.Expression = TextureSampleNode;
			}
			CreatedMaterial->PostEditChange();

			// Offset
			TextureSampleNode->MaterialExpressionEditorX -= 600.0f;

			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreator::TryConnectMetallic(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& MetallicName : MetallicArray)
	{
		if (SelectedTexture->GetName().Contains(MetallicName))
		{
			SelectedTexture->CompressionSettings = TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_LinearColor;

			UMaterialEditorOnlyData* MaterialEditorData = CreatedMaterial->GetEditorOnlyData();
			if (MaterialEditorData)
			{
				// Add the texture sample node to the material graph
				MaterialEditorData->ExpressionCollection.Expressions.Add(TextureSampleNode);

				// Connect the node to Metallic
				FExpressionInput& MetallicInput = CreatedMaterial->GetEditorOnlyData()->Metallic;
				MetallicInput.Expression = TextureSampleNode;
			}

			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.0f;
			TextureSampleNode->MaterialExpressionEditorY += 240;
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreator::TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& RoughnessName : RoughnessArray)
	{
		if (SelectedTexture->GetName().Contains(RoughnessName))
		{
			SelectedTexture->CompressionSettings = TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_LinearColor;

			UMaterialEditorOnlyData* MaterialEditorData = CreatedMaterial->GetEditorOnlyData();
			if (MaterialEditorData)
			{
				// Add the texture sample node to the material graph
				MaterialEditorData->ExpressionCollection.Expressions.Add(TextureSampleNode);

				// Connect pin
				FExpressionInput& RoughnessInput = CreatedMaterial->GetEditorOnlyData()->Roughness;
				RoughnessInput.Expression = TextureSampleNode;
			}

			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.0f;
			TextureSampleNode->MaterialExpressionEditorY += 480;
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreator::TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& NormalName : NormalArray)
	{
		if (SelectedTexture->GetName().Contains(NormalName))
		{
			SelectedTexture->CompressionSettings = TC_Normalmap;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_Normal;

			UMaterialEditorOnlyData* MaterialEditorData = CreatedMaterial->GetEditorOnlyData();
			if (MaterialEditorData)
			{
				// Add the texture sample node to the material graph
				MaterialEditorData->ExpressionCollection.Expressions.Add(TextureSampleNode);

				// Connect pin
				FExpressionInput& NormalInput = CreatedMaterial->GetEditorOnlyData()->Normal;
				NormalInput.Expression = TextureSampleNode;
			}

			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.0f;
			TextureSampleNode->MaterialExpressionEditorY += 720;
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreator::TryConnectAO(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& AOName : AmbientOcclusionArray)
	{
		if (SelectedTexture->GetName().Contains(AOName))
		{
			SelectedTexture->CompressionSettings = TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_LinearColor;

			UMaterialEditorOnlyData* MaterialEditorData = CreatedMaterial->GetEditorOnlyData();
			if (MaterialEditorData)
			{
				// Add the texture sample node to the material graph
				MaterialEditorData->ExpressionCollection.Expressions.Add(TextureSampleNode);

				// Connect pin
				FExpressionInput& AOInput = CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion;
				AOInput.Expression = TextureSampleNode;
			}

			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.0f;
			TextureSampleNode->MaterialExpressionEditorY += 960;
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreator::TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const auto& ORMName : ORMArray)
	{
		if (SelectedTexture->GetName().Contains(ORMName))
		{
			SelectedTexture->CompressionSettings = TC_Masks;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_Masks;

			UMaterialEditorOnlyData* MaterialEditorData = CreatedMaterial->GetEditorOnlyData();
			if (MaterialEditorData)
			{
				// Add the texture sample node to the material graph
				MaterialEditorData->ExpressionCollection.Expressions.Add(TextureSampleNode);

				// Connect pins
				CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.Connect(1, TextureSampleNode);
				CreatedMaterial->GetEditorOnlyData()->Roughness.Connect(2, TextureSampleNode);
				CreatedMaterial->GetEditorOnlyData()->Metallic.Connect(3, TextureSampleNode);
			}
			CreatedMaterial->PostEditChange();

			// Offset
			TextureSampleNode->MaterialExpressionEditorX -= 600.0f;
			TextureSampleNode->MaterialExpressionEditorY += 960;
			
			return true;
		}
	}

	return false;
}

UMaterialInstanceConstant* UQuickMaterialCreator::CreateMaterialInstanceConstant(UMaterial* CreatedMaterial, FString InMaterialName, const FString& Path)
{
	InMaterialName.RemoveFromStart(TEXT("M_"));
	InMaterialName.InsertAt(0, TEXT("MI_"));

	// Load asset tools module
	const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

	// Create Asset and Factory
	UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(InMaterialName, Path, UMaterialInstanceConstant::StaticClass(), Factory);

	if (UMaterialInstanceConstant* MaterialInstance = Cast<UMaterialInstanceConstant>(CreatedAsset))
	{
		MaterialInstance->SetParentEditorOnly(CreatedMaterial);

		MaterialInstance->PostEditChange();
		CreatedMaterial->PostEditChange();

		return MaterialInstance;
	}
	
	return nullptr;
}
